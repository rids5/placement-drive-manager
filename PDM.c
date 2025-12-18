#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_STUDENTS 100
#define MAX_COMPANIES 50
#define MAX_SKILLS 200
#define HASH_SIZE 100


// Student Structure
typedef struct Student {
    int rollNo;
    char name[50];
    float cgpa;
    char skills[MAX_SKILLS];
    bool isPlaced;
    char placedCompany[50];
    float package;
} Student;

// Hash Table Node for Students
typedef struct HashNode {
    Student data;
    struct HashNode* next;
} HashNode;

// Company Structure (for BST)
typedef struct Company {
    char name[50];
    char role[50];
    float package;
    float eligibilityCGPA;
    int requiredStudents;
    struct Company* left;
    struct Company* right;
} Company;

// Application Queue Node
typedef struct AppNode {
    int rollNo;
    char companyName[50];
    struct AppNode* next;
} AppNode;

// Priority Queue for Shortlisting
typedef struct PriorityNode {
    Student student;
    struct PriorityNode* next;
} PriorityNode;

// Drive Schedule (Linked List)
typedef struct Drive {
    char companyName[50];
    char date[20];
    char role[50];
    struct Drive* next;
} Drive;

// Interview Stack Node
typedef struct InterviewNode {
    char round[30];
    int rollNo;
    char status[20];
    struct InterviewNode* next;
} InterviewNode;


HashNode* studentHash[HASH_SIZE] = {NULL};
Company* companyRoot = NULL;
AppNode* applicationQueue = NULL;
PriorityNode* shortlistQueue = NULL;
Drive* driveHead = NULL;
InterviewNode* interviewStack = NULL;


int hashFunction(int rollNo) {
    return rollNo % HASH_SIZE;
}

void insertStudent(Student s) {
    int index = hashFunction(s.rollNo);
    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    newNode->data = s;
    newNode->next = studentHash[index];
    studentHash[index] = newNode;
    printf("✓ Student %s added successfully!\n", s.name);
}

Student* searchStudent(int rollNo) {
    int index = hashFunction(rollNo);
    HashNode* temp = studentHash[index];
    while (temp != NULL) {
        if (temp->data.rollNo == rollNo) {
            return &(temp->data);
        }
        temp = temp->next;
    }
    return NULL;
}

void displayAllStudents() {
    printf("\n========== ALL STUDENTS ==========\n");
    int count = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode* temp = studentHash[i];
        while (temp != NULL) {
            printf("\n%d. Roll No: %d\n", ++count, temp->data.rollNo);
            printf("   Name: %s\n", temp->data.name);
            printf("   CGPA: %.2f\n", temp->data.cgpa);
            printf("   Skills: %s\n", temp->data.skills);
            printf("   Status: %s\n", temp->data.isPlaced ? "PLACED" : "NOT PLACED");
            if (temp->data.isPlaced) {
                printf("   Company: %s\n", temp->data.placedCompany);
                printf("   Package: %.2f LPA\n", temp->data.package);
            }
            temp = temp->next;
        }
    }
    if (count == 0) printf("No students found.\n");
}


Company* createCompany(char name[], char role[], float package, float cgpa, int required) {
    Company* newNode = (Company*)malloc(sizeof(Company));
    strcpy(newNode->name, name);
    strcpy(newNode->role, role);
    newNode->package = package;
    newNode->eligibilityCGPA = cgpa;
    newNode->requiredStudents = required;
    newNode->left = newNode->right = NULL;
    return newNode;
}

Company* insertCompany(Company* root, char name[], char role[], float package, float cgpa, int required) {
    if (root == NULL) {
        return createCompany(name, role, package, cgpa, required);
    }
    if (strcmp(name, root->name) < 0) {
        root->left = insertCompany(root->left, name, role, package, cgpa, required);
    } else {
        root->right = insertCompany(root->right, name, role, package, cgpa, required);
    }
    return root;
}

void inorderCompanies(Company* root) {
    if (root != NULL) {
        inorderCompanies(root->left);
        printf("\n→ Company: %s\n", root->name);
        printf("  Role: %s\n", root->role);
        printf("  Package: %.2f LPA\n", root->package);
        printf("  Min CGPA: %.2f\n", root->eligibilityCGPA);
        printf("  Required Students: %d\n", root->requiredStudents);
        inorderCompanies(root->right);
    }
}

Company* searchCompany(Company* root, char name[]) {
    if (root == NULL || strcmp(root->name, name) == 0) {
        return root;
    }
    if (strcmp(name, root->name) < 0) {
        return searchCompany(root->left, name);
    }
    return searchCompany(root->right, name);
}

// ==================== QUEUE FUNCTIONS (APPLICATIONS) ====================

void enqueueApplication(int rollNo, char companyName[]) {
    AppNode* newNode = (AppNode*)malloc(sizeof(AppNode));
    newNode->rollNo = rollNo;
    strcpy(newNode->companyName, companyName);
    newNode->next = NULL;
    
    if (applicationQueue == NULL) {
        applicationQueue = newNode;
    } else {
        AppNode* temp = applicationQueue;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    printf("✓ Application submitted successfully!\n");
}

void displayApplications() {
    if (applicationQueue == NULL) {
        printf("No applications in queue.\n");
        return;
    }
    printf("\n========== APPLICATION QUEUE ==========\n");
    AppNode* temp = applicationQueue;
    int count = 1;
    while (temp != NULL) {
        printf("%d. Roll No: %d → Company: %s\n", count++, temp->rollNo, temp->companyName);
        temp = temp->next;
    }
}

// ==================== PRIORITY QUEUE (SHORTLISTING) ====================

void insertByPriority(Student s) {
    PriorityNode* newNode = (PriorityNode*)malloc(sizeof(PriorityNode));
    newNode->student = s;
    newNode->next = NULL;
    
    if (shortlistQueue == NULL || shortlistQueue->student.cgpa < s.cgpa) {
        newNode->next = shortlistQueue;
        shortlistQueue = newNode;
    } else {
        PriorityNode* temp = shortlistQueue;
        while (temp->next != NULL && temp->next->student.cgpa >= s.cgpa) {
            temp = temp->next;
        }
        newNode->next = temp->next;
        temp->next = newNode;
    }
}

void generateShortlist(char companyName[]) {
    Company* comp = searchCompany(companyRoot, companyName);
    if (comp == NULL) {
        printf("Company not found!\n");
        return;
    }
    
    // Clear previous shortlist
    while (shortlistQueue != NULL) {
        PriorityNode* temp = shortlistQueue;
        shortlistQueue = shortlistQueue->next;
        free(temp);
    }
    
    // Add eligible students to priority queue
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode* temp = studentHash[i];
        while (temp != NULL) {
            if (!temp->data.isPlaced && temp->data.cgpa >= comp->eligibilityCGPA) {
                insertByPriority(temp->data);
            }
            temp = temp->next;
        }
    }
    
    printf("\n========== SHORTLISTED STUDENTS FOR %s ==========\n", companyName);
    PriorityNode* temp = shortlistQueue;
    int count = 1;
    while (temp != NULL && count <= comp->requiredStudents) {
        printf("%d. %s (Roll: %d, CGPA: %.2f)\n", 
               count++, temp->student.name, temp->student.rollNo, temp->student.cgpa);
        temp = temp->next;
    }
}

// ==================== LINKED LIST (DRIVE SCHEDULING) ====================

void scheduleDrive(char companyName[], char date[], char role[]) {
    Drive* newDrive = (Drive*)malloc(sizeof(Drive));
    strcpy(newDrive->companyName, companyName);
    strcpy(newDrive->date, date);
    strcpy(newDrive->role, role);
    newDrive->next = NULL;
    
    if (driveHead == NULL) {
        driveHead = newDrive;
    } else {
        Drive* temp = driveHead;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newDrive;
    }
    printf("✓ Drive scheduled successfully!\n");
}

void displayDrives() {
    if (driveHead == NULL) {
        printf("No drives scheduled.\n");
        return;
    }
    printf("\n========== UPCOMING PLACEMENT DRIVES ==========\n");
    Drive* temp = driveHead;
    int count = 1;
    while (temp != NULL) {
        printf("\n%d. Company: %s\n", count++, temp->companyName);
        printf("   Date: %s\n", temp->date);
        printf("   Role: %s\n", temp->role);
        temp = temp->next;
    }
}

// ==================== STACK (INTERVIEW ROUNDS) ====================

void pushInterview(char round[], int rollNo, char status[]) {
    InterviewNode* newNode = (InterviewNode*)malloc(sizeof(InterviewNode));
    strcpy(newNode->round, round);
    newNode->rollNo = rollNo;
    strcpy(newNode->status, status);
    newNode->next = interviewStack;
    interviewStack = newNode;
}

void displayInterviewStack() {
    if (interviewStack == NULL) {
        printf("No interview records.\n");
        return;
    }
    printf("\n========== INTERVIEW ROUNDS (LIFO) ==========\n");
    InterviewNode* temp = interviewStack;
    int count = 1;
    while (temp != NULL) {
        printf("%d. Roll: %d | Round: %s | Status: %s\n", 
               count++, temp->rollNo, temp->round, temp->status);
        temp = temp->next;
    }
}

// ==================== REPORT GENERATION ====================

void generateReports() {
    FILE* fp = fopen("placement_report.txt", "w");
    if (fp == NULL) {
        printf("Error creating report file!\n");
        return;
    }
    
    int totalStudents = 0, placedStudents = 0;
    float totalPackage = 0, highestPackage = 0;
    
    fprintf(fp, "========================================\n");
    fprintf(fp, "   PLACEMENT DRIVE REPORT\n");
    fprintf(fp, "========================================\n\n");
    
    time_t now = time(NULL);
    fprintf(fp, "Generated: %s\n", ctime(&now));
    
    fprintf(fp, "\n--- STUDENT PLACEMENT DETAILS ---\n");
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode* temp = studentHash[i];
        while (temp != NULL) {
            totalStudents++;
            fprintf(fp, "\nRoll: %d | Name: %s | CGPA: %.2f\n", 
                    temp->data.rollNo, temp->data.name, temp->data.cgpa);
            if (temp->data.isPlaced) {
                placedStudents++;
                totalPackage += temp->data.package;
                if (temp->data.package > highestPackage) {
                    highestPackage = temp->data.package;
                }
                fprintf(fp, "Status: PLACED at %s | Package: %.2f LPA\n", 
                        temp->data.placedCompany, temp->data.package);
            } else {
                fprintf(fp, "Status: NOT PLACED\n");
            }
            temp = temp->next;
        }
    }
    
    fprintf(fp, "\n--- PLACEMENT STATISTICS ---\n");
    fprintf(fp, "Total Students: %d\n", totalStudents);
    fprintf(fp, "Placed Students: %d\n", placedStudents);
    if (totalStudents > 0) {
        fprintf(fp, "Placement Percentage: %.2f%%\n", 
                (float)placedStudents / totalStudents * 100);
    }
    fprintf(fp, "Highest Package: %.2f LPA\n", highestPackage);
    if (placedStudents > 0) {
        fprintf(fp, "Average Package: %.2f LPA\n", totalPackage / placedStudents);
    }
    
    fclose(fp);
    printf("\n✓ Report generated successfully! Check 'placement_report.txt'\n");
}

// ==================== PLACEMENT FUNCTIONS ====================

void placeStudent(int rollNo, char companyName[], float package) {
    Student* s = searchStudent(rollNo);
    if (s == NULL) {
        printf("Student not found!\n");
        return;
    }
    
    s->isPlaced = true;
    strcpy(s->placedCompany, companyName);
    s->package = package;
    printf("✓ Student %s placed at %s with %.2f LPA\n", s->name, companyName, package);
}

// ==================== MENU SYSTEM ====================

void studentMenu() {
    int choice;
    while (1) {
        printf("\n╔════════════════════════════════╗\n");
        printf("║     STUDENT MANAGEMENT         ║\n");
        printf("╚════════════════════════════════╝\n");
        printf("1. Add Student\n");
        printf("2. Search Student\n");
        printf("3. Display All Students\n");
        printf("4. Back to Main Menu\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        
        if (choice == 1) {
            Student s;
            printf("Enter Roll No: ");
            scanf("%d", &s.rollNo);
            getchar();
            printf("Enter Name: ");
            fgets(s.name, 50, stdin);
            s.name[strcspn(s.name, "\n")] = 0;
            printf("Enter CGPA: ");
            scanf("%f", &s.cgpa);
            getchar();
            printf("Enter Skills: ");
            fgets(s.skills, MAX_SKILLS, stdin);
            s.skills[strcspn(s.skills, "\n")] = 0;
            s.isPlaced = false;
            insertStudent(s);
        } else if (choice == 2) {
            int roll;
            printf("Enter Roll No: ");
            scanf("%d", &roll);
            Student* s = searchStudent(roll);
            if (s) {
                printf("\n→ Found: %s (CGPA: %.2f)\n", s->name, s->cgpa);
            } else {
                printf("Student not found!\n");
            }
        } else if (choice == 3) {
            displayAllStudents();
        } else if (choice == 4) {
            break;
        }
    }
}

void companyMenu() {
    int choice;
    while (1) {
        printf("\n|_________________________________\n");
        printf("|   COMPANY MANAGEMENT            |\n");
        printf("|__________________________________|\n");
        printf("1. Add Company\n");
        printf("2. Display All Companies\n");
        printf("3. Back to Main Menu\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        
        if (choice == 1) {
            char name[50], role[50];
            float pkg, cgpa;
            int req;
            getchar();
            printf("Enter Company Name: ");
            fgets(name, 50, stdin);
            name[strcspn(name, "\n")] = 0;
            printf("Enter Role: ");
            fgets(role, 50, stdin);
            role[strcspn(role, "\n")] = 0;
            printf("Enter Package (LPA): ");
            scanf("%f", &pkg);
            printf("Enter Min CGPA: ");
            scanf("%f", &cgpa);
            printf("Enter Required Students: ");
            scanf("%d", &req);
            companyRoot = insertCompany(companyRoot, name, role, pkg, cgpa, req);
            printf("✓ Company added successfully!\n");
        } else if (choice == 2) {
            printf("\n========= COMPANIES (SORTED) ==========\n");
            inorderCompanies(companyRoot);
        } else if (choice == 3) {
            break;
        }
    }
}

void placementMenu() {
    int choice;
    while (1) {
        printf("\n╔════════════════════════════════╗\n");
        printf("║    PLACEMENT OPERATIONS        ║\n");
        printf("╚════════════════════════════════╝\n");
        printf("1. Submit Application\n");
        printf("2. View Applications\n");
        printf("3. Generate Shortlist\n");
        printf("4. Schedule Drive\n");
        printf("5. View Scheduled Drives\n");
        printf("6. Add Interview Round\n");
        printf("7. View Interview Stack\n");
        printf("8. Place Student\n");
        printf("9. Back to Main Menu\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        
        if (choice == 1) {
            int roll;
            char comp[50];
            printf("Enter Roll No: ");
            scanf("%d", &roll);
            getchar();
            printf("Enter Company Name: ");
            fgets(comp, 50, stdin);
            comp[strcspn(comp, "\n")] = 0;
            enqueueApplication(roll, comp);
        } else if (choice == 2) {
            displayApplications();
        } else if (choice == 3) {
            char comp[50];
            getchar();
            printf("Enter Company Name: ");
            fgets(comp, 50, stdin);
            comp[strcspn(comp, "\n")] = 0;
            generateShortlist(comp);
        } else if (choice == 4) {
            char comp[50], date[20], role[50];
            getchar();
            printf("Enter Company Name: ");
            fgets(comp, 50, stdin);
            comp[strcspn(comp, "\n")] = 0;
            printf("Enter Date (DD-MM-YYYY): ");
            fgets(date, 20, stdin);
            date[strcspn(date, "\n")] = 0;
            printf("Enter Role: ");
            fgets(role, 50, stdin);
            role[strcspn(role, "\n")] = 0;
            scheduleDrive(comp, date, role);
        } else if (choice == 5) {
            displayDrives();
        } else if (choice == 6) {
            int roll;
            char round[30], status[20];
            printf("Enter Roll No: ");
            scanf("%d", &roll);
            getchar();
            printf("Enter Round (Aptitude/Technical/HR): ");
            fgets(round, 30, stdin);
            round[strcspn(round, "\n")] = 0;
            printf("Enter Status (Cleared/Failed): ");
            fgets(status, 20, stdin);
            status[strcspn(status, "\n")] = 0;
            pushInterview(round, roll, status);
            printf("✓ Interview record added!\n");
        } else if (choice == 7) {
            displayInterviewStack();
        } else if (choice == 8) {
            int roll;
            char comp[50];
            float pkg;
            printf("Enter Roll No: ");
            scanf("%d", &roll);
            getchar();
            printf("Enter Company Name: ");
            fgets(comp, 50, stdin);
            comp[strcspn(comp, "\n")] = 0;
            printf("Enter Package (LPA): ");
            scanf("%f", &pkg);
            placeStudent(roll, comp, pkg);
        } else if (choice == 9) {
            break;
        }
    }
}

int main() {
    int choice;
    
    printf("|_______________________________________|\n");
    printf("|  PLACEMENT DRIVE MANAGER SYSTEM       |\n");
    printf("|   by Coding Titans                    |\n");
    printf("|_______________________________________|\n");
    
    while (1) {
        printf("\n_______________________________\n");
        printf("|        MAIN MENU              |\n");
        printf("|_______________________________|\n");
        printf("1. Student Management\n");
        printf("2. Company Management\n");
        printf("3. Placement Operations\n");
        printf("4. Generate Reports\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                studentMenu();
                break;
            case 2:
                companyMenu();
                break;
            case 3:
                placementMenu();
                break;
            case 4:
                generateReports();
                break;
            case 5:
                printf("\nThank you for using Placement Drive Manager!\n");
                return 0;
            default:
                printf("Invalid choice!\n");
        }
    }
    
    return 0;
}