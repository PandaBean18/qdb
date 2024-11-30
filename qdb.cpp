#include <iostream>
#include <cstring>

using namespace std;

typedef struct Column {
    const char* name = NULL;
    const char* type = NULL;
} Column;

typedef struct Table {
    const char* name = NULL;
    int numCols = 0;
    Column **cols = NULL;
} Table;

typedef struct Query {
    const char* type;
    Table* table;
} Query;

typedef struct ParsedString {
    char** words; 
    int wordCount;
} ParsedString;

class NoTableName : public exception 
{
    public:
    const char* what() const noexcept override
    {
        return "Could not parse table name from query\n";
    }
};

void parseString(char* inp, ParsedString* p) {
    int n = 0;
    char **splitVals = NULL;
    int k = 0;
    char *current = NULL;

    for(char* c = inp; 1; c++) {
        if ((*c == ' ' || *c == '\0')  && (current != NULL)) {
            // if this is the case, we increment the value of n, then allocate n amount of space in temp and then copy the the values of 
            // splitVals into it and then assign the value of pointer to current at (n-1)th spot.

            n++;
            char **temp = new char*[n];

            for(int i = 0; i < (n-1); i++) {
                temp[i] = splitVals[i];
            }

            temp[n-1] = current;
            splitVals = temp;
            current = NULL;
            k = 0;

            if (*c == '\0') {
                break;
            }

        } else if (*c != ' ' && *c != '\0' && *c != ',' && *c != '(' && *c != ')') {
            if (current == NULL) {
                current = new char[2];
                current[0] = *c;
                current[1] = '\0';
                k = 2;
            } else {
                k++;
                char *temp = new char[k];
                for(int i = 0; i < (k-2); i++) {
                    temp[i] = current[i];
                }

                temp[k-2] = *c;
                temp[k-1] = '\0';
                current = temp;
            }
        } else {
            if (*c == '\0') {
                break;
            } else {
                continue;
            }
        }
    }

    (*p).wordCount = n;
    (*p).words = splitVals;
}

void parseQuery(char* userQuery, Query* q) {
    ParsedString p = {0, 0};
    parseString(userQuery, &p);

    if (strcmp(p.words[0], "SELECT") == 0) {
        q->type = "SELECT";
        q->table = new Table;

        int found = 0;

        for(int i = 0; i < p.wordCount; i++) {
            if (strcmp(p.words[i], "FROM") == 0) {
                q->table->name = p.words[i+1];
                found = 1;
                break;
            }
        }

        if (!found) {
            throw NoTableName();
        }

        return;
    } else if (strcmp(p.words[0], "CREATE") == 0) {
        q->type = "CREATE";
        q->table = new Table;

        if (p.wordCount < 2) {
            throw NoTableName();
        }

        q->table->name = p.words[2];
        q->table->numCols = 0;
        Column **cols = NULL;

        for(int i = 3; i < p.wordCount; i += 2) {
            q->table->numCols++;
            Column **temp = new Column*[i-2];

            for(int j = 0; j < (i-3); j++) {
                temp[j] = cols[j];
            }

            temp[q->table->numCols-1] = new Column;
            temp[q->table->numCols-1]->name = p.words[i];
            temp[q->table->numCols-1]->type = p.words[i+1];
            cols = temp;
        }
        q->table->cols = cols;
        return;
    } else if (strcmp(p.words[0], "INSERT") == 0) {
        q->type = "INSERT";
        q->table = new Table;

        if (p.wordCount < 2) {
            throw NoTableName();
        }

        q->table->name = p.words[2];
        return;
    }
}

int main() {
    char c[] = "CREATE TABLE users (id INT, name TEXT)"; 
    Query q = {0, 0};

    parseQuery(c, &q);

    cout << "Query Type: " << q.type << endl;
    cout << "Table Name: " << q.table->name << endl;
    cout << q.table->numCols << endl;

    if (strcmp(q.type, "CREATE") == 0) {
        for(int i = 0; i < q.table->numCols; i++) {
            cout << q.table->cols[i]->name << endl;
            cout << q.table->cols[i]->type << endl;
        }
    }

    return 0;
}