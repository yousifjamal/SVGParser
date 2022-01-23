#include "SVGParser.h"
#include "SVGHelpers.h"


SVG* createSVG(const char* fileName){
    xmlDoc* newFile = NULL;
    xmlNode* rootElement = NULL;
    SVG* newSVG;

    newFile = xmlReadFile(fileName, NULL, 0);               //Read xml File then check if it was read properly
    if (newFile == NULL || strcmp(fileName, "") == 0){
        return NULL;
    }

    rootElement = xmlDocGetRootElement(newFile);            //Grab the root element of the XML doc

    newSVG = malloc(sizeof(SVG));                       //allocate memory for SVG struct

    extractMetaInfo(newSVG, rootElement);
    newSVG->otherAttributes = extractAttributes(rootElement, attributeToString, deleteAttribute, compareAttributes);
    newSVG->rectangles = extractRectangles(rootElement, attributeToString, deleteAttribute, compareAttributes, rectangleToString, deleteRectangle, compareRectangles);                                        
    newSVG->circles = extractCircles(rootElement, attributeToString, deleteAttribute, compareAttributes, circleToString, deleteCircle, compareCircles);
    newSVG->paths = extractPaths(rootElement);
    newSVG->groups = extractGroups(rootElement);


    xmlFreeDoc(newFile);
    xmlCleanupParser();

    return newSVG;
}

char* SVGToString(const SVG* img){
    char* output;
    char* temp;
    if (img == NULL){
        return "";
    }
    output = malloc(strlen(img->title) + strlen(img->description) + strlen(img->namespace) + 56);

    sprintf(output, "-SVG Element-\nTitle: %s\nDescription: %s\nNamespace: %s\n", img->title, img->description, img->namespace);
    temp = toString(img->otherAttributes);
    output = realloc(output, strlen(output) + strlen(temp) + 20);  //20 including the -Svg attributes- header
    strcat(output, "\n-SVG Attributes-\n");
    strcat(output, temp);
    free(temp);
    temp = toString(img->rectangles);
    output = realloc(output, strlen(output) + strlen(temp) +20);        //null terminators + written text
    strcat(output, "\n-SVG Rectangles-\n\n");
    strcat(output, temp);
    free(temp);
    temp = toString(img->circles);
    output = realloc(output, strlen(output) + strlen(temp) + 18);
    strcat(output, "\n-SVG Circles-\n\n");
    strcat(output, temp);
    free(temp);
    temp = toString(img->paths);
    output = realloc(output, strlen(output) + strlen(temp) + 16);
    strcat(output, "\n-SVG Paths-\n\n");
    strcat(output, temp);
    free(temp);
    temp = toString(img->groups);
    output = realloc(output, strlen(output) + strlen(temp) + 17);
    strcat(output, "\n-SVG Groups-\n\n");
    strcat(output, temp);
    free(temp);

    return output;
    
}

void deleteSVG(SVG* img){
    freeList(img->groups);
    freeList(img->paths);
    freeList(img->circles);
    freeList(img->rectangles);
    freeList(img->otherAttributes);
    free(img);
}



//~~~HELPERS FOR LIST API~~~
//Attributes
int compareAttributes(const void *first, const void *second){       //change to actually compare attributes
    Attribute* firstAttr = (Attribute*)first;
    Attribute* secondAttr = (Attribute*)second;

    return strcmp(firstAttr->name, secondAttr->name) + strcmp(firstAttr->value, secondAttr->value);

}

void deleteAttribute( void* data){                                  //change to actually list delete aswell
    Attribute* deletedNode;

    deletedNode = (Attribute*)(data);
    free(deletedNode->name);

    free(deletedNode);
}

char* attributeToString(void* data){
    Attribute* nodeToPrint;
    char* tempStr;

    if (data == NULL){
        return "";
    }

    nodeToPrint = (Attribute*)data;
    tempStr = (char*)malloc(sizeof(char) * (strlen(nodeToPrint->value) + strlen(nodeToPrint->name) + 10));
    sprintf(tempStr, "%s: %s\n", nodeToPrint->name, nodeToPrint->value);

    return tempStr;
}

//Rectangles
void deleteRectangle(void* data){
    Rectangle* rectToDelete;

    rectToDelete = (Rectangle*)data;
    freeList(rectToDelete->otherAttributes);
    free(rectToDelete);
}
char* rectangleToString(void* data){
    Rectangle* rectToString;
    char* tempStr;
    char* attrTempStr;

    if (data == NULL){
        return "";
    }

    rectToString = (Rectangle*)data;
    attrTempStr = toString(rectToString->otherAttributes);
    tempStr = malloc(sizeof(Rectangle) + strlen(attrTempStr) + 81);
    sprintf(tempStr, "<rect>\nx: %f\ny: %f\nwidth: %f\nheight: %f\nUnits: %s\n~OtherAttributes~\n%s</rect>\n\n", rectToString->x, rectToString->y, rectToString->width, rectToString->height, rectToString->units, attrTempStr);
    free(attrTempStr);

    return tempStr;
}
int compareRectangles(const void *first, const void *second){
    Rectangle* firstRect = (Rectangle*)first;
    Rectangle* secondRect = (Rectangle*)second;

    return (int)((firstRect->x - secondRect->x) + (firstRect->y - secondRect->y) + (firstRect->width - secondRect->width) + (firstRect->height - secondRect->height));
}

//Circles
void deleteCircle(void* data){
    Circle* circToDelete = (Circle*)data;

    freeList(circToDelete->otherAttributes);
    free(circToDelete);
}
char* circleToString(void* data){
    Circle* circToPrint = (Circle*)data;
    char* circleAttributes;
    char* circleString;

    if (data == NULL){
        return "";
    }

    circleAttributes = toString(circToPrint->otherAttributes);
    circleString = malloc(sizeof(Circle) + strlen(circleAttributes) + 76);
    sprintf(circleString, "<circle>\ncx = %f\ncy = %f\nr = %f\nUnits: %s\n~Other Attributes~\n%s</circle>\n\n", circToPrint->cx, circToPrint->cy, circToPrint->r, circToPrint->units, circleAttributes);
    free(circleAttributes);

    return circleString;

}
int compareCircles(const void *first, const void *second){
    Circle* circ1 = (Circle*)first;
    Circle* circ2 = (Circle*)second;

    return(int)((circ1->cx - circ2->cx) + (circ1->cy - circ2->cy) + (circ1->r - circ2->r));
}

//Path
void deletePath(void* data){
    Path* pathToDelete = (Path*)data;

    freeList(pathToDelete->otherAttributes);
    free(pathToDelete);
}
char* pathToString(void* data){
    if (data == NULL){
        return "";
    }
    Path* pathTopPrint = (Path*)data;
    char* pathAttributes;
    char* printPath;

    pathAttributes = toString(pathTopPrint->otherAttributes);
    printPath = malloc(strlen(pathTopPrint->data) + strlen(pathAttributes) + 42);
    sprintf(printPath, "<p>\nData: %s\n~Other Attributes~\n%s</p>\n\n", pathTopPrint->data, pathAttributes); //finish helpers
    free(pathAttributes);

    return printPath;

}
int comparePaths(const void *first, const void *second){
    Path* p1 = (Path*)first;
    Path*p2 = (Path*)second;

    return strcmp(p1->data, p2->data);
}

//Group
void deleteGroup(void* data){
    Group* groupToDelete = (Group*)data;

    freeList(groupToDelete->rectangles);
    freeList(groupToDelete->circles);
    freeList(groupToDelete->paths);
    freeList(groupToDelete->groups);
    freeList(groupToDelete->otherAttributes);
    free(groupToDelete);

}
char* groupToString(void* data){
    if (data == NULL){
        return "";
    }
    Group* groupToPrint = (Group*)data;
    char* printedString;
    char* rectGroups = toString(groupToPrint->rectangles);
    char* circleGroups = toString(groupToPrint->circles);
    char* pathGroups = toString(groupToPrint->paths);
    char* depthGroups = toString(groupToPrint->groups);
    char* attrGroups = toString(groupToPrint->otherAttributes);

    printedString = malloc(sizeof(Group) + strlen(rectGroups) + strlen(circleGroups) + strlen(pathGroups) + strlen(depthGroups) + strlen(attrGroups) + 44);
    sprintf(printedString, "<g>%s\n%s\n%s\n%s\n~Other Attributes~\n%s</g>\n\n", rectGroups, circleGroups, pathGroups, depthGroups, attrGroups);

    free(rectGroups);
    free(circleGroups);
    free(pathGroups);
    free(depthGroups);
    free(attrGroups);

    return printedString;
}
int compareGroups(const void *first, const void *second){  
    Group* g1 = (Group*)first;
    Group* g2 = (Group*)second;
    int result;

    char* n1;
    char* n2;

    n1 = toString(g1->rectangles);      //If all the strings are the same it must be the same group element
    n2 = toString(g2->rectangles);
    result = strcmp(n1,n2);             //returns 0 if group1 = group 2
    free(n1);
    free(n2);
    n1 = toString(g1->circles);
    n2 = toString(g1->circles);
    result += strcmp(n1,n2);
    free(n1);
    free(n2);
    n1 = toString(g1->paths);
    n2 = toString(g1->paths);
    result += strcmp(n1,n2);
    free(n1);
    free(n2);
    n1 = toString(g1->groups);
    n2 = toString(g1->groups);
    result += strcmp(n1,n2);
    free(n1);
    free(n2);
    n1 = toString(g1->otherAttributes);
    n2 = toString(g1->otherAttributes);
    result += strcmp(n1,n2);
    free(n1);
    free(n2);

    return result;
}

//Get Functionality
//Get Rects
List* getRects(const SVG* img){
    if (img == NULL){
        return NULL;
    }
    List* returnedList = initializeList(rectangleToString, masterDelete, compareRectangles);
    ListIterator i = createIterator(img->groups);
    void* element;

    //Check each group in img->groups for all rectangles(recursively because groups can contain groups)
    while((element = nextElement(&i))!= NULL){
        digForRects(returnedList, element);
    }
    
    //Check SVG rectangles
    i = createIterator(img->rectangles);
    while ((element = nextElement(&i))!= NULL){
        insertBack(returnedList, element);
    }

    return returnedList;
}

//getCircles
List* getCircles(const SVG* img){
    if (img == NULL){
        return NULL;
    }
    List* returnedList = initializeList(circleToString, masterDelete, compareCircles);
    ListIterator i = createIterator(img->groups);
    void* element;

    //Check each group in img->groups for all Cicles(recursively because groups can contain groups)
    while((element = nextElement(&i))!= NULL){
        digForCircles(returnedList, element);
    }
    
    //Check SVG Circles
    i = createIterator(img->circles);
    while ((element = nextElement(&i))!= NULL){
        insertBack(returnedList, element);
    }

    return returnedList;
}

//getPaths
List* getPaths(const SVG* img){
    if (img == NULL){
        return NULL;
    }
    List* returnedList = initializeList(pathToString, masterDelete, comparePaths);
    ListIterator i = createIterator(img->groups);
    void* element;

    while((element = nextElement(&i))!= NULL){
        digForPaths(returnedList, element);
    }

    i = createIterator(img->paths);
    while((element = nextElement(&i))!= NULL){
        insertBack(returnedList, element);
    }
    
    return returnedList;
}
//getGroups
List* getGroups(const SVG* img){
    if (img == NULL){
        return NULL;
    }
    List* returnedList = initializeList(groupToString, masterDelete, compareGroups);
    ListIterator i = createIterator(img->groups);
    void* element;

    while((element = nextElement(&i))!= NULL){  //grab nested groups first
        digForGroups(returnedList, element);
    }

    i = createIterator(img->groups);            //grab svg groups next
    while((element = nextElement(&i))!= NULL){
        insertBack(returnedList, element);
    }
    
    return returnedList;
}

//Summaries
//Num Rects with area
int numRectsWithArea(const SVG* img, float area){
    int matches = 0;
    if (img == NULL ||area < 0){
        return matches;
    }
    List* allRects = getRects(img);
    ListIterator i = createIterator(allRects);      //grab list of circles
    Rectangle* rectElement;
    

    while((rectElement = (Rectangle*)nextElement(&i)) != NULL){
        if (ceil(rectElement->height * rectElement->width) == area){        //iterate through and perform comparison
            matches++;
        }
    }
    freeList(allRects);     //free pointer list afterwards
    return matches;
}
//Num Circles with area
int numCirclesWithArea(const SVG* img, float area){
    int matches = 0;
    if (img == NULL || area < 0){
        return matches;
    }
    List* allCicles = getCircles(img);
    ListIterator i = createIterator(allCicles);
    Circle* circElement;

    while((circElement = (Circle*)nextElement(&i)) != NULL){
        if (ceil(3.1415926536 * pow(circElement->r, 2)) == area){
            matches++;
        }
    }
    freeList(allCicles);
    return matches;
}
//Num Paths with Data
int numPathsWithdata(const SVG* img, const char* data){
    int matches = 0;
    if(img == NULL || !strcmp(data, "") || data == NULL){
        return matches;
    }
    List* allPaths = getPaths(img);
    ListIterator i = createIterator(allPaths);
    Path* pathElement;

    while((pathElement = (Path*)nextElement(&i)) != NULL){
        if (!strcmp(pathElement->data, data)){
            matches++;
        }
    }
    freeList(allPaths);
    return matches;
}
//Num Groups with len
int numGroupsWithLen(const SVG* img, int len){
    int matches = 0;
    if (img == NULL|| len <= 0){
        return matches;
    }
    List* allGroups = getGroups(img);
    ListIterator i = createIterator(allGroups);
    Group* groupElement;

    while ((groupElement = nextElement(&i))!= NULL){
        if ((getLength(groupElement->circles) + getLength(groupElement->rectangles) + getLength(groupElement->paths) + getLength(groupElement->groups)) == len){
            matches++;
        }
    }
    freeList(allGroups);
    return matches;
}