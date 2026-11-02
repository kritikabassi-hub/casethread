#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEXT 100

/* ---------- DATA STRUCTURES ---------- */

typedef struct {
    int id;
    char title[TEXT];
    char category[TEXT];
    char location[TEXT];
    char date[20];
    char status[30];
} Case;

typedef struct {
    int id;
    int caseID;
    char description[TEXT];
    char location[TEXT];
    char collectionTime[30];
    char collector[TEXT];
} Evidence;

typedef struct {
    int id;
    int caseID;
    char description[TEXT];
    int timestamp;
} Event;

/* ---------- GLOBAL DATA ---------- */

Case *cases = NULL;
Evidence *evidence = NULL;
Event *events = NULL;

int caseCount = 0;
int evidenceCount = 0;
int eventCount = 0;

/* ---------- INPUT FUNCTIONS ---------- */

void readLine(char text[], int size) {
    if (fgets(text, size, stdin) != NULL) {
        text[strcspn(text, "\n")] = '\0';
    }
}

void readText(const char *message, char text[], int size) {
    printf("%s", message);
    readLine(text, size);
}

int readInt(const char *message) {
    char input[50];
    char extra;
    int value;

    while (1) {
        printf("%s", message);
        readLine(input, sizeof(input));

        if (sscanf(input, "%d %c", &value, &extra) == 1) {
            return value;
        }

        printf("Invalid input! Enter a whole number.\n");
    }
}

/* ---------- CASE REGISTRY ---------- */

int findCase(int id) {
    int i;

    for (i = 0; i < caseCount; i++) {
        if (cases[i].id == id) {
            return i;
        }
    }

    return -1;
}

void registerCase(void) {
    Case *temp;
    Case newCase;

    printf("\n--- REGISTER NEW CASE ---\n");

    newCase.id = readInt("Enter Case ID: ");

    if (newCase.id <= 0 || findCase(newCase.id) != -1) {
        printf("Invalid or duplicate Case ID!\n");
        return;
    }

    readText("Enter case title: ",
             newCase.title, TEXT);

    readText("Enter category: ",
             newCase.category, TEXT);

    readText("Enter location: ",
             newCase.location, TEXT);

    readText("Enter date (DD-MM-YYYY): ",
             newCase.date, sizeof(newCase.date));

    strcpy(newCase.status, "Under Review");

    temp = realloc(cases,
                   (caseCount + 1) * sizeof(Case));

    if (temp == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    cases = temp;
    cases[caseCount] = newCase;
    caseCount++;

    printf("Case registered successfully!\n");
}

void viewCases(void) {
    int i;

    printf("\n========== REGISTERED CASES ==========\n");

    if (caseCount == 0) {
        printf("No cases registered.\n");
        return;
    }

    for (i = 0; i < caseCount; i++) {
        printf("\nCase ID: %d\n", cases[i].id);
        printf("Title: %s\n", cases[i].title);
        printf("Category: %s\n", cases[i].category);
        printf("Location: %s\n", cases[i].location);
        printf("Date: %s\n", cases[i].date);
        printf("Status: %s\n", cases[i].status);
    }
}

void searchCase(void) {
    int id, index;

    id = readInt("Enter Case ID to search: ");
    index = findCase(id);

    if (index == -1) {
        printf("Case not found.\n");
        return;
    }

    printf("\nCase found!\n");
    printf("ID: %d\n", cases[index].id);
    printf("Title: %s\n", cases[index].title);
    printf("Category: %s\n", cases[index].category);
    printf("Location: %s\n", cases[index].location);
    printf("Date: %s\n", cases[index].date);
    printf("Status: %s\n", cases[index].status);
}

void updateCase(void) {
    int id, index, choice;

    id = readInt("Enter Case ID to update: ");
    index = findCase(id);

    if (index == -1) {
        printf("Case not found.\n");
        return;
    }

    printf("\n1. Update title\n");
    printf("2. Update location\n");
    printf("3. Update status\n");

    choice = readInt("Enter choice: ");

    switch (choice) {
        case 1:
            readText("Enter new title: ",
                     cases[index].title, TEXT);
            break;

        case 2:
            readText("Enter new location: ",
                     cases[index].location, TEXT);
            break;

        case 3:
            printf("1. Under Review\n");
            printf("2. Closed\n");
            printf("3. Pending\n");

            choice = readInt("Select status: ");

            if (choice == 1)
                strcpy(cases[index].status, "Under Review");
            else if (choice == 2)
                strcpy(cases[index].status, "Closed");
            else if (choice == 3)
                strcpy(cases[index].status, "Pending");
            else {
                printf("Invalid status choice.\n");
                return;
            }
            break;

        default:
            printf("Invalid choice.\n");
            return;
    }

    printf("Case updated successfully!\n");
}

void deleteCase(void) {
    int id, index, i, confirm;

    id = readInt("Enter Case ID to delete: ");
    index = findCase(id);

    if (index == -1) {
        printf("Case not found.\n");
        return;
    }

    printf("Deleting a case also removes its associated\n");
    printf("evidence and timeline records.\n");

    confirm = readInt("Confirm deletion? (1=Yes, 0=No): ");

    if (confirm != 1) {
        printf("Deletion cancelled.\n");
        return;
    }

    for (i = 0; i < evidenceCount; ) {
        if (evidence[i].caseID == id) {
            memmove(&evidence[i], &evidence[i + 1],
                    (evidenceCount - i - 1) * sizeof(Evidence));
            evidenceCount--;
        } else {
            i++;
        }
    }

    for (i = 0; i < eventCount; ) {
        if (events[i].caseID == id) {
            memmove(&events[i], &events[i + 1],
                    (eventCount - i - 1) * sizeof(Event));
            eventCount--;
        } else {
            i++;
        }
    }

    memmove(&cases[index], &cases[index + 1],
            (caseCount - index - 1) * sizeof(Case));

    caseCount--;

    if (caseCount == 0) {
        free(cases);
        cases = NULL;
    }

    if (evidenceCount == 0) {
        free(evidence);
        evidence = NULL;
    }

    if (eventCount == 0) {
        free(events);
        events = NULL;
    }

    printf("Case and associated records deleted.\n");
}

void caseMenu(void) {
    int choice;

    do {
        printf("\n========== CASE REGISTRY ==========\n");
        printf("1. Register New Case\n");
        printf("2. View All Cases\n");
        printf("3. Search Case\n");
        printf("4. Update Case\n");
        printf("5. Delete Case\n");
        printf("0. Return to Main Menu\n");

        choice = readInt("Enter choice: ");

        switch (choice) {
            case 1: registerCase(); break;
            case 2: viewCases(); break;
            case 3: searchCase(); break;
            case 4: updateCase(); break;
            case 5: deleteCase(); break;
            case 0: break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 0);
}

/* ---------- EVIDENCE ANALYZER ---------- */

void addEvidence(void) {
    Evidence item;
    Evidence *temp;

    printf("\n--- ADD EVIDENCE ---\n");

    item.id = readInt("Enter Evidence ID: ");

    if (item.id <= 0) {
        printf("Evidence ID must be positive.\n");
        return;
    }

    {
        int i;

        for (i = 0; i < evidenceCount; i++) {
            if (evidence[i].id == item.id) {
                printf("Duplicate Evidence ID!\n");
                return;
            }
        }
    }

    item.caseID = readInt("Enter associated Case ID: ");

    if (findCase(item.caseID) == -1) {
        printf("Register the case before adding evidence.\n");
        return;
    }

    readText("Enter evidence description: ",
             item.description, TEXT);

    readText("Enter collection location: ",
             item.location, TEXT);

    readText("Enter collection date/time: ",
             item.collectionTime, sizeof(item.collectionTime));

    readText("Enter collector identification: ",
             item.collector, TEXT);

    temp = realloc(evidence,
                   (evidenceCount + 1) * sizeof(Evidence));

    if (temp == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    evidence = temp;
    evidence[evidenceCount] = item;
    evidenceCount++;

    printf("Evidence record added successfully.\n");
}

void viewEvidence(void) {
    int i;

    printf("\n========== EVIDENCE RECORDS ==========\n");

    if (evidenceCount == 0) {
        printf("No evidence records found.\n");
        return;
    }

    for (i = 0; i < evidenceCount; i++) {
        printf("\nEvidence ID: %d\n", evidence[i].id);
        printf("Case ID: %d\n", evidence[i].caseID);
        printf("Description: %s\n", evidence[i].description);
        printf("Location: %s\n", evidence[i].location);
        printf("Collection date/time: %s\n",
               evidence[i].collectionTime);
        printf("Collector: %s\n", evidence[i].collector);
    }
}

void analyzeEvidence(void) {
    int caseID, i, found = 0;
    int missing = 0;

    caseID = readInt("Enter Case ID to analyze: ");

    if (findCase(caseID) == -1) {
        printf("Case not found.\n");
        return;
    }

    printf("\n--- EVIDENCE DOCUMENTATION REPORT ---\n");

    for (i = 0; i < evidenceCount; i++) {
        if (evidence[i].caseID == caseID) {
            found = 1;
            printf("\nEvidence ID: %d\n", evidence[i].id);

            if (strlen(evidence[i].description) == 0) {
                printf("[!] Missing description\n");
                missing++;
            }

            if (strlen(evidence[i].location) == 0) {
                printf("[!] Missing collection location\n");
                missing++;
            }

            if (strlen(evidence[i].collectionTime) == 0) {
                printf("[!] Missing collection date/time\n");
                missing++;
            }

            if (strlen(evidence[i].collector) == 0) {
                printf("[!] Missing collector identification\n");
                missing++;
            }

            if (strlen(evidence[i].description) > 0 &&
                strlen(evidence[i].location) > 0 &&
                strlen(evidence[i].collectionTime) > 0 &&
                strlen(evidence[i].collector) > 0) {
                printf("Required fields are present.\n");
            }
        }
    }

    if (!found) {
        printf("No evidence registered for this case.\n");
    } else {
        printf("\nMissing documentation fields: %d\n", missing);
        printf("This report checks field completeness only.\n");
    }
}

void evidenceMenu(void) {
    int choice;

    do {
        printf("\n====== EVIDENCE DOCUMENTATION ======\n");
        printf("1. Add Evidence\n");
        printf("2. View Evidence\n");
        printf("3. Analyze Documentation\n");
        printf("0. Return to Main Menu\n");

        choice = readInt("Enter choice: ");

        switch (choice) {
            case 1: addEvidence(); break;
            case 2: viewEvidence(); break;
            case 3: analyzeEvidence(); break;
            case 0: break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 0);
}

/* ---------- TIMELINE AUDITOR ---------- */

void addEvent(void) {
    Event item;
    Event *temp;

    printf("\n--- ADD TIMELINE EVENT ---\n");

    item.id = readInt("Enter Event ID: ");

    if (item.id <= 0) {
        printf("Event ID must be positive.\n");
        return;
    }

    {
        int i;

        for (i = 0; i < eventCount; i++) {
            if (events[i].id == item.id) {
                printf("Duplicate Event ID!\n");
                return;
            }
        }
    }

    item.caseID = readInt("Enter associated Case ID: ");

    if (findCase(item.caseID) == -1) {
        printf("Register the case first.\n");
        return;
    }

    readText("Enter event description: ",
             item.description, TEXT);

    item.timestamp = readInt(
        "Enter numeric event time (e.g. 1810 for 18:10): ");

    if (item.timestamp < 0 || item.timestamp > 2359 ||
        item.timestamp % 100 >= 60) {
        printf("Invalid time. Use 0 to 2359 in HHMM format.\n");
        return;
    }

    temp = realloc(events,
                   (eventCount + 1) * sizeof(Event));

    if (temp == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    events = temp;
    events[eventCount] = item;
    eventCount++;

    printf("Timeline event added successfully.\n");
}

void auditTimeline(void) {
    int caseID, i, j, found = 0;
    Event temp;

    caseID = readInt("Enter Case ID to audit: ");

    if (findCase(caseID) == -1) {
        printf("Case not found.\n");
        return;
    }

    /* Sort matching events by time using bubble sort. */
    for (i = 0; i < eventCount - 1; i++) {
        for (j = 0; j < eventCount - i - 1; j++) {
            if (events[j].caseID == caseID &&
                events[j + 1].caseID == caseID &&
                events[j].timestamp > events[j + 1].timestamp) {

                temp = events[j];
                events[j] = events[j + 1];
                events[j + 1] = temp;
            }
        }
    }

    printf("\n========== TIMELINE REPORT ==========\n");

    for (i = 0; i < eventCount; i++) {
        if (events[i].caseID == caseID) {
            found = 1;

            printf("\nEvent ID: %d\n", events[i].id);
            printf("Time: %02d:%02d\n",
                   events[i].timestamp / 100,
                   events[i].timestamp % 100);
            printf("Description: %s\n",
                   events[i].description);

            for (j = i + 1; j < eventCount; j++) {
                if (events[j].caseID == caseID &&
                    events[j].timestamp == events[i].timestamp) {
                    printf("[!] Same recorded time as Event ID %d\n",
                           events[j].id);
                }
            }
        }
    }

    if (!found) {
        printf("No events registered for this case.\n");
    }

    printf("\nNote: Matching times are flagged for review;\n");
    printf("they do not automatically indicate a contradiction.\n");
}

void timelineMenu(void) {
    int choice;

    do {
        printf("\n========== TIMELINE AUDITOR ==========\n");
        printf("1. Add Timeline Event\n");
        printf("2. Audit and Sort Timeline\n");
        printf("0. Return to Main Menu\n");

        choice = readInt("Enter choice: ");

        switch (choice) {
            case 1: addEvent(); break;
            case 2: auditTimeline(); break;
            case 0: break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 0);
}

/* ---------- INVESTIGATION SUMMARY ---------- */

void investigationSummary(void) {
    printf("\n======= INVESTIGATION SUMMARY =======\n");
    printf("Total cases: %d\n", caseCount);
    printf("Total evidence records: %d\n", evidenceCount);
    printf("Total timeline events: %d\n", eventCount);
    printf("System status: Operational\n");
    printf("Storage: Temporary in-memory records\n");
}

/* ---------- MAIN FUNCTION ---------- */

int main(void) {
    int choice;

    do {
        printf("\n");
        printf("============================================\n");
        printf("                 CASETHREAD                 \n");
        printf("       INVESTIGATION SUPPORT SYSTEM         \n");
        printf("============================================\n");
        printf("1. Case Registry\n");
        printf("2. Evidence Documentation Analyzer\n");
        printf("3. Timeline Auditor\n");
        printf("4. Investigation Summary\n");
        printf("0. Exit\n");
        printf("============================================\n");

        choice = readInt("Enter your choice: ");

        switch (choice) {
            case 1: caseMenu(); break;
            case 2: evidenceMenu(); break;
            case 3: timelineMenu(); break;
            case 4: investigationSummary(); break;
            case 0: printf("Exiting CASETHREAD. Goodbye!\n"); break;
            default: printf("Invalid choice! Try again.\n");
        }
    } while (choice != 0);

    free(cases);
    free(evidence);
    free(events);

    return 0;
}
