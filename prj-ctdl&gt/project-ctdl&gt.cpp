#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_PRODUCTS 1000
#define MAX_ITEMS_PER_ORDER 100

typedef struct Product {
    int idProduct;
    char nameProduct[50];
    int quantity;
    float price;
} Product;

typedef struct Order {
    char orderID[20];
    char customerName[50];
    Product products[MAX_ITEMS_PER_ORDER];
    int productCount;
    double totalPrice;
    struct Order* next;
} Order;

typedef struct Queue {
    Order* front;
    Order* rear;
} Queue;

void initQueue(Queue* queue) {
    queue->front = NULL;
    queue->rear = NULL;
}

int isEmpty(Queue* queue) {
    return queue->front == NULL;
}

void enqueue(Queue* queue, Order* newOrder) {
    if (queue->rear == NULL) {
        queue->front = newOrder;
        queue->rear = newOrder;
    } else {
        queue->rear->next = newOrder;
        queue->rear = newOrder;
    }
    newOrder->next = NULL; 
}

Order* dequeue(Queue* queue) {
    if (isEmpty(queue)) {
        printf("Hang doi rong. Khong co don hang de xu ly.\n");
        return NULL;
    }

    Order* temp = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    return temp;
}

int isOrderIDExist(Queue* queue, const char* orderID) {
    Order* temp = queue->front;
    while (temp != NULL) {
        if (strcmp(temp->orderID, orderID) == 0) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

void readOrdersFromFile(Queue* queue, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Khong the mo file %s de doc.\n", filename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        Order* newOrder = (Order*)malloc(sizeof(Order));
        if (newOrder == NULL) {
            printf("Khong the cap phat bo nho.\n");
            fclose(file);
            return;
        }

        if (sscanf(line, "Ma don hang: %s", newOrder->orderID) != 1) {
            free(newOrder);
            continue;
        }

        if (!fgets(line, sizeof(line), file) || sscanf(line, "Ten khach hang: %[^]", newOrder->customerName) != 1) {
            free(newOrder);
            continue;
        }

        newOrder->productCount = 0;
        if (!fgets(line, sizeof(line), file) || strcmp(line, "Danh sach san pham:\n") != 0) {
            free(newOrder);
            continue;
        }

        while (fgets(line, sizeof(line), file) && strstr(line, "- ")) {
            sscanf(line, "- %[^,], So luong: %d, Gia: %f",
                   newOrder->products[newOrder->productCount].nameProduct,
                   &newOrder->products[newOrder->productCount].quantity,
                   &newOrder->products[newOrder->productCount].price);
            newOrder->productCount++;
        }

        if (sscanf(line, "Tong gia: %lf", &newOrder->totalPrice) != 1) {
            free(newOrder);
            continue;
        }

        newOrder->next = NULL;
        enqueue(queue, newOrder);

        fgets(line, sizeof(line), file);
    }

    fclose(file);
}

void cancelOrderByID(Queue* queue) {
    if (isEmpty(queue)) {
        printf("Hang doi rong. Khong co don hang de huy.\n");
        return;
    }

    char orderID[20];
    printf("Nhap ma don hang de huy: ");
    scanf("%s", orderID);

    Order* temp = queue->front;
    Order* prev = NULL;

    while (temp != NULL && strcmp(temp->orderID, orderID) != 0) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Khong tim thay don hang voi ma: %s\n", orderID);
        return;
    }

    if (prev == NULL) {
        queue->front = temp->next;
    } else {
        prev->next = temp->next;
    }

    if (temp == queue->rear) {
        queue->rear = prev;
    }

    free(temp);
    printf("Don hang voi ma %s da duoc huy thanh cong.\n", orderID);
}

void displayProductList(Product productList[], int size) {
    printf("\nDanh sach san pham:\n");
    for (int i = 0; i < size; i++) {
        printf("%d. Ma san pham: %d | Ten: %s | So luong: %d | Gia: %.2f\n",
               i + 1,
               productList[i].idProduct,
               productList[i].nameProduct,
               productList[i].quantity,
               productList[i].price);
    }
}

void chooseProducts(Product productList[], int size, Product* selectedProducts, int* productCount) {
    int choice, quantity;
    char continueChoice;

    *productCount = 0;

    do {
        displayProductList(productList, size);
        printf("Ch?n s?n ph?m theo s? th? t? (1-%d): ", size);
        
        while (getchar() != '\n'); 

        if (scanf("%d", &choice) != 1) {
            printf("Lua ch?n không h?p l?. Vui lòng nh?p m?t s?.\n");
            while (getchar() != '\n'); 
            continue;
        }

        if (choice < 1 || choice > size) {
            printf("L?a ch?n không h?p l?. Vui lòng ch?n l?i.\n");
            continue;
        }

        Product chosen = productList[choice - 1];

        if (chosen.quantity <= 0) {
            printf("S?n ph?m %s dã h?t hàng. Vui lòng ch?n s?n ph?m khác.\n", chosen.nameProduct);
            continue;
        }

        printf("B?n dã ch?n: %s\n", chosen.nameProduct);

        do {
            printf("Nh?p s? lu?ng (t?i da %d): ", chosen.quantity);
            if (scanf("%d", &quantity) != 1) {
                printf("Vui lòng nh?p m?t s?.\n");
                while (getchar() != '\n');
                quantity = -1; 
                continue;
            }

            if (quantity > chosen.quantity || quantity <= 0) {
                printf("So luong khong hop le. Vui long nhap lai.\n");
            }
        } while (quantity > chosen.quantity || quantity <= 0);

        chosen.quantity = quantity;
        selectedProducts[*productCount] = chosen;
        productList[choice - 1].quantity -= quantity; 
        (*productCount)++;

        printf("Ban co muon chon thêm san pham? (y/n): ");
        while (getchar() != '\n');
        scanf("%c", &continueChoice);

    } while (continueChoice == 'y' || continueChoice == 'Y');
}




void addOrder(Queue* queue, Product productList[], int size) {
    Order* newOrder = (Order*)malloc(sizeof(Order));
    if (!newOrder) {
        printf("Khong the cap phat bo nho.\n");
        return;
    }

    do {
        printf("Nhap ma don hang: ");
        scanf("%s", newOrder->orderID);

        if (isOrderIDExist(queue, newOrder->orderID)) {
            printf("Ma don hang da ton tai. Vui long nhap lai.\n");
        }
    } while (isOrderIDExist(queue, newOrder->orderID));

    printf("Nhap ten khach hang: ");
    scanf(" %[^]", newOrder->customerName);

    chooseProducts(productList, size, newOrder->products, &newOrder->productCount);

    if (newOrder->productCount == 0) {
        printf("Khong co san pham nao duoc chon. Don hang khong duoc tao.\n");
        free(newOrder);
        return;
    }

    newOrder->totalPrice = 0;
    for (int i = 0; i < newOrder->productCount; i++) {
        newOrder->totalPrice += newOrder->products[i].price * newOrder->products[i].quantity;
    }

    newOrder->next = NULL;
    enqueue(queue, newOrder);
    printf("Don hang da duoc them thanh cong.\n");
}

void writeOrderToFile(Order* order, const char* filename) {
    FILE* file = fopen(filename, "a");
    if (file == NULL) {
        printf("Khong the mo file %s de ghi.\n", filename);
        return;
    }

    fprintf(file, "Ma don hang: %s\n", order->orderID);
    fprintf(file, "Ten khach hang: %s\n", order->customerName);
    fprintf(file, "Danh sach san pham:\n");
    for (int i = 0; i < order->productCount; i++) {
        fprintf(file, "- %s, So luong: %d, Gia: %.2f\n",
                order->products[i].nameProduct,
                order->products[i].quantity,
                order->products[i].price);
    }
    fprintf(file, "Tong gia: %.2f\n", order->totalPrice);
    fprintf(file, "-----------------------------\n");

    fclose(file);
}

void saveAllOrdersToFile(Queue* queue, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Khong the mo file %s de ghi.\n", filename);
        return;
    }

    Order* temp = queue->front;
    while (temp != NULL) {
        fprintf(file, "Ma don hang: %s\n", temp->orderID);
        fprintf(file, "Ten khach hang: %s\n", temp->customerName);
        fprintf(file, "Danh sach san pham:\n");
        for (int i = 0; i < temp->productCount; i++) {
            fprintf(file, "- %s, So luong: %d, Gia: %.2f\n",
                    temp->products[i].nameProduct,
                    temp->products[i].quantity,
                    temp->products[i].price);
        }
        fprintf(file, "Tong gia: %.2f\n", temp->totalPrice);
        fprintf(file, "-----------------------------\n");
        temp = temp->next;
    }

    fclose(file);
}

void processAndWriteOrders(Queue* queue, const char* filename) {
    if (isEmpty(queue)) {
        printf("Hang doi rong. Khong co don hang de ghi vao file.\n");
        return;
    }

    while (!isEmpty(queue)) {
        Order* order = dequeue(queue);
        if (order != NULL) {
            printf("Xu ly va ghi don hang: %s\n", order->orderID);
            writeOrderToFile(order, filename);
            free(order);
        }
    }
    printf("Tat ca don hang da duoc xu thanh cong.\n");
}

void displayOrders(Queue* queue) {
    if (queue->front == NULL) {
        printf("Hang doi rong.\n");
        return;
    }

    Order* temp = queue->front;
    printf("\nDanh sach don hang:\n");
    while (temp != NULL) {
        printf("-----------------------------\n");
        printf("Ma don hang: %s\n", temp->orderID);
        printf("Ten khach hang: %s\n", temp->customerName);
        printf("Danh sach san pham:\n");
        for (int i = 0; i < temp->productCount; i++) {
            printf("- %s, So luong: %d, Gia: %.2f\n",
                   temp->products[i].nameProduct,
                   temp->products[i].quantity,
                   temp->products[i].price);
        }
        printf("Tong gia: %.2f\n", temp->totalPrice);
        temp = temp->next;
    }
    printf("-----------------------------\n");
}

void freeQueue(Queue* queue) {
    while (!isEmpty(queue)) {
        Order* temp = dequeue(queue);
        free(temp);
    }
}

int main() {
    Product productList[MAX_PRODUCTS] = {
        {1001, "Laptop", 50, 1500.0},
        {1002, "Mouse", 200, 20.0},
        {1003, "Keyboard", 100, 45.0},
        {1004, "Monitor", 30, 300.0},
        {1005, "Headphones", 80, 100.0}
    };

    Queue queue;
    initQueue(&queue);

    int choice = 0;
    const char* orders = "order.txt";
    const char* orderSuccessfully = "processed_order.txt";

    readOrdersFromFile(&queue, orders);

    do {
        printf("\n=== MENU ===\n");
        printf("1. Them vao don hang\n");
        printf("2. Hien thi danh sach don hang\n");
        printf("3. Xu ly don hang\n");
        printf("4. Huy don hang\n");
        printf("5. Thoat\n");
        printf("Nhap lua chon cua ban: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addOrder(&queue, productList, 5);
                break;
            case 2:
                displayOrders(&queue);
                break;
            case 3:
                processAndWriteOrders(&queue, orderSuccessfully);
                break;
            case 4:
                cancelOrderByID(&queue);
                break;
            case 5:
                saveAllOrdersToFile(&queue, orders);
                printf("Tat ca don hang da duoc luu vao file %s.\n", orders);
                freeQueue(&queue);
                printf("Thoat chuong trinh.\n");
                break;
            default:
                printf("Lua chon khong hop le. Vui long chon lai.\n");
        }
    } while (choice != 5);

    return 0;
}

