#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define QT_NODE_CAPACITY 4
#define MAX_ARRAY_SIZE 1000

typedef struct Point {
    int val;
    float x;
    float y;
} Point;

typedef struct BoundryBox {
    Point *center;
    float halfDimension;    
} BoundryBox;

typedef struct QuadTree {
    BoundryBox *boundry;
    Point **points;

    struct QuadTree* NW;
    struct QuadTree* NE;
    struct QuadTree* SW;
    struct QuadTree* SE;

} QuadTree;

//Helping functions
Point *Point_new(float x, float y);
BoundryBox *BoundryBox_new(Point *center, float halfDimension);
bool BoundryBox_cotains_point(BoundryBox *boundry, Point *point);
bool BoundryBox_intersects_BoundryBox(BoundryBox *self, BoundryBox *other);
int QuadTree_points_size(Point *points[]);
bool Is_LeafNode(QuadTree quad);

//Creates a Quadtree Node
QuadTree *QuadTree_new(BoundryBox *boundry);
bool QuadTree_insert(QuadTree *root, Point *point);

//Divides a Quadtree in 4 sub Nodes
QuadTree *QuadTree_subdivide(QuadTree *root);
//Deletes a point
int Delete_Point(QuadTree *root,Point * point);
//updates value of a point
bool Point_Update(QuadTree *root,Point * point);

//Searching functions
bool Point_Search(QuadTree *root,Point * point);
Point **QuadTree_query_range(QuadTree *root, BoundryBox *range);

//Functions for display
void Point_print(Point *point);
void BoundryBox_print(BoundryBox *box);
void display_QuadTree(QuadTree *quad);


Point *Point_new(float x, float y) {
    Point *p = (Point *)malloc(sizeof(Point));
    p->x = x;
    p->y = y;
    p->val = __INT_MAX__;
    return p;
}

void Point_print(Point *point) {
    printf("(%2.2f, %2.2f)\n", point->x, point->y);
    printf("Value: %d\n",point->val);
}

BoundryBox *BoundryBox_new(Point *center, float halfDimension) {
    BoundryBox *box = (BoundryBox *)malloc(sizeof(BoundryBox));
    box->center = center;
    box->halfDimension = halfDimension;
    return box;
}

bool BoundryBox_cotains_point(BoundryBox *boundry, Point *point) {
    if (point->x < boundry->center->x - boundry->halfDimension || point->x > boundry->center->x + boundry->halfDimension) {
        return false;
    }

    if (point->y < boundry->center->y - boundry->halfDimension || point->y > boundry->center->y + boundry->halfDimension) {
        return false;
    }

    return true;
}

bool BoundryBox_intersects_BoundryBox(BoundryBox *self, BoundryBox *other) {
    if (self->center->x + self->halfDimension > other->center->x - other->halfDimension) {
        if (self->center->x - self->halfDimension < other->center->x + other->halfDimension) {
            if (self->center->y + self->halfDimension > other->center->y - other->halfDimension) {
                if (self->center->y - self->halfDimension < other->center->y + other->halfDimension) {
                    return true;
                }
            }
        }
    }
    return false;
}

void BoundryBox_print(BoundryBox *box) {
    printf("\n");
    printf("(%3.2f, %3.2f)---------(%3.2f, %3.2f)\n", box->center->x - box->halfDimension, box->center->y + box->halfDimension, box->center->x + box->halfDimension, box->center->y + box->halfDimension);
    printf("    |                           |\n");
    printf("    |                           |\n");
    printf("    |                           |\n");
    printf("    |                           |\n");
    printf("(%3.2f, %3.2f)---------(%3.2f, %3.2f)\n", box->center->x - box->halfDimension, box->center->y - box->halfDimension, box->center->x + box->halfDimension, box->center->y - box->halfDimension);
    printf("\n");
}

QuadTree *QuadTree_new(BoundryBox *boundry) {
    QuadTree *qt = (QuadTree *)malloc(sizeof(QuadTree));
    qt->NE = NULL;
    qt->NW = NULL;
    qt->SE = NULL;
    qt->SW = NULL;

    qt->boundry = boundry;

    qt->points = (Point **)malloc(sizeof(Point*) * QT_NODE_CAPACITY);

    for (size_t i = 0; i < QT_NODE_CAPACITY; i++)
    {
        qt->points[i] = NULL;
    }

    return qt;
}

bool Is_LeafNode(QuadTree quad){
    if(quad.NW == NULL && quad.NE == NULL &&quad.SW == NULL && quad.SE == NULL)
        return true;
    return false;
}

int QuadTree_points_size(Point *points[]) {
    int i;
    for (i = 0; i < QT_NODE_CAPACITY; i++)
    {
        if (points[i] == NULL) {
            return i;
        }
    }

    return i;
}

QuadTree *QuadTree_subdivide(QuadTree *root) {
    if(!Is_LeafNode(*root)){
        printf("Already Subdivided\n");
        return root;
    }
    float halfDim = root->boundry->halfDimension / 2;
    // North West
    Point *nw_p = Point_new(root->boundry->center->x - halfDim, root->boundry->center->y + halfDim);
    root->NW = QuadTree_new(BoundryBox_new(nw_p, halfDim));
    
    // North East
    Point *ne_p = Point_new(root->boundry->center->x + halfDim, root->boundry->center->y + halfDim);
    root->NE = QuadTree_new(BoundryBox_new(ne_p, halfDim));
    
    // South West
    Point *sw_p = Point_new(root->boundry->center->x - halfDim, root->boundry->center->y - halfDim);
    root->SW = QuadTree_new(BoundryBox_new(sw_p, halfDim));
    
    // South East
    Point *se_p = Point_new(root->boundry->center->x + halfDim, root->boundry->center->y - halfDim);
    root->SE = QuadTree_new(BoundryBox_new(se_p, halfDim));

    return root;
}


bool QuadTree_insert(QuadTree *root, Point *point) {
    if (!BoundryBox_cotains_point(root->boundry, point)) {
        return false;
    }
    
    int points_size = QuadTree_points_size(root->points);
    
    if (points_size < QT_NODE_CAPACITY && root->NW == NULL) {
        printf("points size : %d\n",points_size);
        root->points[points_size] = point;
        return true;
    }

    if (root->NW == NULL) {
        QuadTree_subdivide(root);
    }

    if (QuadTree_insert(root->NW, point)) return true;
    if (QuadTree_insert(root->NE, point)) return true;
    if (QuadTree_insert(root->SW, point)) return true;
    if (QuadTree_insert(root->SE, point)) return true;

    return false;
}

Point **QuadTree_query_range(QuadTree *root, BoundryBox *range) {
    Point **result;
    result = (Point **)malloc(sizeof(Point *) * MAX_ARRAY_SIZE);

    int index = 0;
    for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
        result[i] = NULL;
    }
    
    if (!BoundryBox_intersects_BoundryBox(root->boundry, range)) {
        return result;
    }

    int points_size = QuadTree_points_size(root->points);
    for (int i = 0; i < points_size; i++)
    {
        if (BoundryBox_cotains_point(range, root->points[i])) {
            result[index++] = root->points[i];
        }
    }
    
    if (root->NW == NULL) {
        return result;
    }

    int i;

    i = 0;
    Point **nw_r = QuadTree_query_range(root->NW, range);
    while (nw_r[i] != NULL && i < MAX_ARRAY_SIZE) {
        result[index++] = nw_r[i++];
    }

    i = 0;
    Point **ne_r = QuadTree_query_range(root->NE, range);
    while (ne_r[i] != NULL && i < MAX_ARRAY_SIZE) {
        result[index++] = ne_r[i++];
    }

    i = 0;
    Point **sw_r = QuadTree_query_range(root->SW, range);
    while (sw_r[i] != NULL && i < MAX_ARRAY_SIZE) {
        result[index++] = sw_r[i++];
    }

    i = 0;
    Point **se_r = QuadTree_query_range(root->SE, range);
    while (se_r[i] != NULL && i < MAX_ARRAY_SIZE) {
        result[index++] = se_r[i++];
    }

    return result;
}

bool Point_Search(QuadTree *root,Point * point){
    if(root == NULL){
        return false;
    }
    if (!BoundryBox_cotains_point(root->boundry,point)){
        // printf("Point not found\n");
        return false;
    }
    for(int i=0;i<QT_NODE_CAPACITY;i++){
        if(root->points[i]!=NULL && root->points[i]->x == point->x && root->points[i]->y == point->y){
            return true;
        }
    }
    if (root->NW==NULL)
        return false;

    if(BoundryBox_cotains_point(root->NW->boundry,point))
        Point_Search(root->NW,point);        
    else if(BoundryBox_cotains_point(root->NE->boundry,point))
        Point_Search(root->NE,point);
    else if(BoundryBox_cotains_point(root->SW->boundry,point))
        Point_Search(root->SW,point);
    else if(BoundryBox_cotains_point(root->SE->boundry,point))
        Point_Search(root->SE,point);
    else 
    return false;
}

int Delete_Point(QuadTree *root,Point * point){
   if(root == NULL){
        printf("Point is not present\n");
        return false;
    }
    if (!BoundryBox_cotains_point(root->boundry,point)){
        printf("Point not found\n");
        return false;
    }
    for(int i=0;i<QT_NODE_CAPACITY;i++){
        if(root->points[i]!=NULL && root->points[i]->x == point->x && root->points[i]->y == point->y){
            Point_print(root->points[i]);
            printf("Deleted\n");
            free(root->points[i]);
            root->points[i]=NULL;
            //shifts the rest values by 1 place
            for(int j=i;j<QT_NODE_CAPACITY-1;j++){
                root->points[j]=root->points[j+1];
                root->points[j+1]=NULL;
            }
            return true;
        }
    }
    if(root->NW!=NULL && BoundryBox_cotains_point(root->NW->boundry,point))
        return Delete_Point(root->NW,point);        
    else if(root->NE!=NULL && BoundryBox_cotains_point(root->NE->boundry,point))
        return Delete_Point(root->NE,point);
    else if(root->SW!=NULL && BoundryBox_cotains_point(root->SW->boundry,point))
        return Delete_Point(root->SW,point);
    else if( root->SE != NULL)
        return Delete_Point(root->SE,point);
    else{
        printf("Point is not present\n");
        return false;
    }
}

bool Point_Update(QuadTree *root,Point * point){
    if(root == NULL){
        printf("Point is not present\n");
        return false;
    }
    if (!BoundryBox_cotains_point(root->boundry,point)){
        printf("Point not found\n");
        return false;
    }
    for(int i=0;i<QT_NODE_CAPACITY;i++){
        if(root->points[i]!=NULL && root->points[i]->x == point->x && root->points[i]->y == point->y){
            printf("Enter the new val:");
            scanf("%d",&(root->points[i]->val));
            return true;
        }
    }
    if(root->NW!=NULL && BoundryBox_cotains_point(root->NW->boundry,point))
        Point_Update(root->NW,point);        
    else if(root->NE!=NULL && BoundryBox_cotains_point(root->NE->boundry,point))
        Point_Update(root->NE,point);
    else if(root->SW!=NULL && BoundryBox_cotains_point(root->SW->boundry,point))
        Point_Update(root->SW,point);
    else if( root->SE != NULL)
        Point_Update(root->SE,point);
    else{
        printf("Point is not present\n");
        return false;
    }
}
void display_QuadTree(QuadTree *quad){
    Point **res = QuadTree_query_range(quad,quad->boundry);
    printf("The Points:\n");
    int j = 0;
    while (res[j] != NULL && j < MAX_ARRAY_SIZE) {
        Point_print(res[j]);
        j++;
    }
    printf("\n");
}
int main()
{
    int choice;
    QuadTree *qt = NULL;

    printf("\n1. Create Quad Tree\n");
    printf("2. Insert Points\n");
    printf("3. Range Query\n");
    printf("4. Search Point\n");
    printf("5. Update Point\n");
    printf("6. Delete Point\n");
    printf("7. Check if Leaf Node\n");
    printf("8. Subdivide Node\n");
    printf("9. Display QuadTree\n");
    printf("10. Exit\n");

    while(true){
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice){
            case 1: {
                printf("Quad Tree\n\n");
                printf("Specify Initial Bounding Box \n");
                printf("Center: \n");
                float x, y;
                printf("X: ");
                scanf("%f", &x);
                printf("Y: ");
                scanf("%f", &y);

                float hd;
                printf("Half Dimension: ");
                scanf("%f", &hd);

                Point *center = Point_new(x, y);
                BoundryBox *boundry = BoundryBox_new(center, hd);
                printf("Quad Tree Boundary\n");
                BoundryBox_print(boundry);

                qt = QuadTree_new(boundry);
                break;
            }
            case 2: {
                if (qt == NULL) {
                    printf("Quad Tree not created yet. Create the Quad Tree first!\n");
                    break;
                }
                int count;
                printf("Enter Number of Points to insert: ");
                scanf("%d", &count);
                int i = 1;
                while (count > 0) {
                    printf("Point %d\n", i);
                    float x, y;
                    printf("X: ");
                    scanf("%f", &x);
                    printf("Y: ");
                    scanf("%f", &y);
                    Point *p = Point_new(x, y);
                    printf("Val: ");
                    scanf("%d", &p->val);
                    if (Point_Search(qt, p)){
                        printf("Point exists previously\n");
                    }else if (!QuadTree_insert(qt, p)){
                        printf("Point outside boundary. Not inserted\n");
                    }else{
                        printf("Point inserted successfully\n");
                        i++;
                        count--;
                    }
                }
                break;
            }
            case 3: {
                if (qt == NULL) {
                    printf("Quad Tree not created yet. Create the Quad Tree first!\n");
                    break;
                }
                printf("\nRange Query\n\n");
                printf("Specify Axis Aligned Bounding Box for Range Query\n");
                printf("Center: \n");
                float x_r, y_r;
                printf("X: ");
                scanf("%f", &x_r);
                printf("Y: ");
                scanf("%f", &y_r);

                float hd_r;
                printf("Half Dimension: ");
                scanf("%f", &hd_r);

                // Manage the hd if provided hd exceeds boundary
                if(x_r+hd_r>(qt->boundry->center->x) + (qt->boundry->halfDimension)){
                    hd_r=(qt->boundry->center->x) + (qt->boundry->halfDimension)-x_r;
                }if(y_r+hd_r>(qt->boundry->center->y) + (qt->boundry->halfDimension)){
                    hd_r=(qt->boundry->center->y) + (qt->boundry->halfDimension)-y_r;
                }
                
                Point *center_r = Point_new(x_r, y_r);
                BoundryBox *boundry_r = BoundryBox_new(center_r, hd_r);
                printf("Range Search Boundary\n");
                BoundryBox_print(boundry_r);
                Point **res = QuadTree_query_range(qt, boundry_r);
                printf("Result\n");
                int j = 0;
                while (res[j] != NULL && j < MAX_ARRAY_SIZE) {
                    Point_print(res[j]);
                    j++;
                }
                break;
            }
            case 4: {
                float a, b;
                printf("Enter the point:\n");
                printf("X: ");
                scanf("%f", &a);
                printf("Y: ");
                scanf("%f", &b);

                Point *new_point = Point_new(a, b);

                if (Point_Search(qt, new_point)) {
                    printf("Point found.\n");
                } else {
                    printf("Point not found.\n");
                }
                break;
            }
            case 5: {
                float a, b;
                printf("Enter the point to update:\n");
                printf("X: ");
                scanf("%f", &a);
                printf("Y: ");
                scanf("%f", &b);

                Point *new_point = Point_new(a, b);

                if (Point_Update(qt, new_point)) {
                    printf("Point updated successfully.\n");
                } else {
                    printf("Point not found or update failed.\n");
                }
                break;
            }
            case 6: {
                float a, b;
                printf("Enter the point to delete:\n");
                printf("X: ");
                scanf("%f", &a);
                printf("Y: ");
                scanf("%f", &b);

                Point *new_point = Point_new(a, b);

                if (Delete_Point(qt, new_point)) {
                    printf("Point deleted successfully.\n");
                } else {
                    printf("Point not found or deletion failed.\n");
                }
                break;
            }
            case 7: {  
                // Check if a node is a leaf node
                if (Is_LeafNode(*qt)) {
                    printf("The current root node is a leaf node.\n");
                } else {
                    printf("The current root node is not a leaf node.\n");
                }
                break;
            }
            case 8: {  
                // Subdivide the node
                if (qt == NULL) {
                    printf("Quad Tree not created yet. Create the Quad Tree first!\n");
                    break;
                }
                if (QuadTree_subdivide(qt)) {
                    printf("Node subdivided successfully.\n");
                } else {
                    printf("Failed to subdivide the node.\n");
                }
                break;
            }
            case 9: {  
                // Display the QuadTree
                if (qt == NULL) {
                    printf("Quad Tree not created yet. Create the Quad Tree first!\n");
                    break;
                }
                printf("Displaying QuadTree:\n");
                display_QuadTree(qt);  
                break;
            }
            case 10: {
                exit(1);
                printf("Exiting program.\n");
                break;
            }
            default: {
                printf("Invalid choice. Please try again.\n");
                break;
            }
        }
    }

    return 0;
}