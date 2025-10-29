#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct product {
    int id;
    char name[50];
    float price;
    int quantity;
};

int getMenuChoice() {
    int choice;
    while(1){
        printf("Enter choice:");
        if(scanf("%d",&choice)!=1){
            printf("Invalid.Enter a number:\n");
            while(getchar() != '\n');
        }
        else{
            return choice;
        }
    }
}

int inputSize() {
    int size;
    printf("Enter initial number of products:");
    while(scanf("%d", &size)!=1 || size<1 || size>100){
        while (getchar()!='\n');
        printf("Invalid.Enter again(1-100):");
    }
    return size;
}

int isValidName(char *name) {
    if(name==NULL) return 0;
    while(*name==' ') name++;
    if(strlen(name)==0) return 0;
    for(int i=0;name[i]!='\0';i++) {
        if(!isalpha(name[i]) && name[i] != ' ')
            return 0;
    }
    return 1;
}

void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++){
        str[i] = tolower(str[i]);
    }    
}

int isDuplicateId(struct product *inv,int size,int id) {
    for (int i=0; i<size; i++) {
        if(inv[i].id==id) return 1;
    }
    return 0;
}

void inputProduct(struct product *p, struct product *inv, int size) {
    while(1){
        printf("Product ID: ");
        if(scanf("%d", &p->id) != 1 || p->id <= 0) {
            while (getchar() != '\n');
            printf("Invalid ID\n");
            continue;
        }
        if(isDuplicateId(inv, size, p->id)) {
            printf("Duplicate ID not allowed\n");
            continue;
        }
        break;
    }

    while(getchar() != '\n');
    while(1){
        printf("Product Name:");
        fgets(p->name, 50, stdin);
        p->name[strcspn(p->name, "\n")] = '\0';
        if(isValidName(p->name)) break;
        printf("Invalid name\n");
    }

    while(1) {
        printf("Product Price: ");
        if(scanf("%f", &p->price) != 1 || p->price < 0) {
            while (getchar() != '\n');
            printf("Invalid price\n");
            continue;
        }
        break;
    }

    while(1) {
        printf("Product Quantity: ");
        if(scanf("%d", &p->quantity) != 1 || p->quantity < 0) {
            while (getchar() != '\n');
            printf("Invalid quantity\n");
            continue;
        }
        break;
    }
}

void addProduct(struct product **inv, int *size) {
    *inv = realloc(*inv, (*size + 1) * sizeof(struct product));
    inputProduct(&(*inv)[*size], *inv, *size);
    (*size)++;
    printf("Product added\n");
}

void viewProducts(struct product *inv, int size) {
    if(size==0) {
        printf("No Products\n");
        return;
    }
    for(int i=0; i<size; i++) {
        printf("ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
               inv[i].id, inv[i].name, inv[i].price, inv[i].quantity);
    }
}

void updateQuantity(struct product *inv, int size) {
    if (size==0) {
        printf("No Products\n");
        return;
    }
    int pid;
    printf("Enter Product ID: ");
    while(scanf("%d", &pid) != 1) {
        while (getchar() != '\n');
        printf("Invalid input! Enter a valid Product ID: ");
    }
    for(int i=0; i<size; i++) {
        if (inv[i].id==pid) {
            int updatedQuantity;
            while(1) {
                printf("Enter new Quantity: ");
                if(scanf("%d", &updatedQuantity) != 1 || updatedQuantity < 0) {
                    while (getchar() != '\n');
                    printf("Invalid quantity\n");
                    continue;
                }
                break;
            }
            inv[i].quantity=updatedQuantity;
            printf("Quantity updated successfully!\n");
            return;
        }
    }
    printf("Product not found\n");
}

void searchProductId(struct product *inv, int size) {
    if(size==0) {
        printf("No Products\n");
        return;
    }
    int pid;
    printf("Enter Product ID: ");
    while(scanf("%d", &pid) != 1) {
        while (getchar() != '\n');
        printf("Invalid.Enter again:");
    }
    for(int i= 0;i<size;i++) {
        if (inv[i].id == pid) {
            printf("ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   inv[i].id, inv[i].name, inv[i].price, inv[i].quantity);
            return;
        }
    }
    printf("No Products found\n");
}

void searchProductName(struct product *inv, int size) {
    if(size==0) {
        printf("No Products\n");
        return;
    }
    char pName[50], temp[50];
    printf("Enter product name to search:");
    while(getchar() != '\n');
    fgets(pName, 50, stdin);
    pName[strcspn(pName, "\n")] = '\0';
    toLowerCase(pName);
    int found=0;
    for(int i = 0; i < size; i++) {
        strcpy(temp, inv[i].name);
        toLowerCase(temp);
        if(strstr(temp, pName)) {
            printf("ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",inv[i].id, inv[i].name, inv[i].price, inv[i].quantity);
            found=1;
        }
    }
    if(!found){
        printf("No Products found\n");
    }    
}

void searchProductPrice(struct product *inv, int size) {
    if(size==0) {
        printf("No Products\n");
        return;
    }

    float minPrice, maxPrice;
    printf("Enter minimum price: ");
    while(scanf("%f", &minPrice) != 1 || minPrice < 0) {
        while (getchar() != '\n');
        printf("Invalid.Enter valid minprice:");
    }

    printf("Enter maximum price: ");
    while(scanf("%f", &maxPrice) != 1 || maxPrice < minPrice) {
        while (getchar() != '\n');
        printf("Invalid.maxprice should be more than minprice.Enter again:");
    }

    int found=0;
    for(int i=0;i<size;i++) {
        if (inv[i].price >= minPrice && inv[i].price <= maxPrice) {
            printf("ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   inv[i].id, inv[i].name, inv[i].price, inv[i].quantity);
            found=1;
        }
    }
    if(!found){
        printf("No products found\n");
    }
}

void deleteProduct(struct product *inv, int *size) {
    if (*size == 0) {
        printf("No Products\n");
        return;
    }
    int pid;
    printf("Enter Product ID to delete:");
    while (scanf("%d", &pid) != 1) {
        while (getchar() != '\n');
        printf("Invalid ID! Enter again: ");
    }
    int index=-1;
    for(int i=0;i<*size; i++) {
        if (inv[i].id == pid) {
            index=i;
            break;
        }
    }
    if(index==-1) {
        printf("Product ID not found\n");
        return;
    }
    for(int i=index; i<*size-1;i++){
        inv[i] = inv[i + 1];
    }     
    (*size)--;
    printf("Product deleted successfully!\n");
}

int main(){
    struct product *inv;
    int size = inputSize();
    inv=calloc(size, sizeof(struct product));

    for(int i = 0; i < size; i++) {
        printf("\nEnter details of Product %d\n", i + 1);
        inputProduct(&inv[i], inv, i);
    }


    while(1){
        printf("\n1.Add New Product\n2.View All Products\n3.Update Quantity\n4.Search Product by ID\n5.Search Product by Name\n6. Search Product by Price Range\n7.Delete Product\n8.Exit\n");
        int choice = getMenuChoice();

        switch (choice) {
            case 1:
                addProduct(&inv, &size);
                break;
            case 2:
                viewProducts(inv, size);
                break;
            case 3: 
                updateQuantity(inv, size); 
                break;
            case 4: 
                searchProductId(inv, size); 
                break;
            case 5: 
                searchProductName(inv, size); 
                break;
            case 6: 
                searchProductPrice(inv, size); 
                break;
            case 7: 
                deleteProduct(inv, &size); 
                break;
            case 8: 
                free(inv); 
                printf("Memory released successfully. Exiting program...");
                return 0;
            default:
                printf("Invalid choice!\n");
        }
    }
}
