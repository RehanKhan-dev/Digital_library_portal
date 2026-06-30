/*
 * ================================================================
 *  DIGITAL LIBRARY PORTAL
 *  ----------------------------------------------------------------
 *  Data Structures : Singly Linked List (catalogue)
 *                    Queue via Linked List (per-book waiting lists)
 *  Paradigm        : Object-Oriented Programming (OOP)
 *  Memory          : Manual – new / delete (no STL containers)
 *
 *  Compile : g++ -std=c++14 -Wall -Wextra -o library digital_library.cpp
 *  Run     : ./library
 * ================================================================
 */

#include <iostream>
#include <cstring>    // strncpy, strlen
#include <cctype>     // tolower, isdigit
#include <cstdlib>    // atoi

using namespace std;

// ----------------------------------------------------------------
//  COMPILE-TIME CONSTANTS
// ----------------------------------------------------------------
static const int MAX_LEN = 128;    // maximum characters for any field
static const int SEP_W   = 60;     // width of separator lines

// ================================================================
//  UTILITY FUNCTIONS
// ================================================================

// Safe bounded string copy – ALWAYS null-terminates dest
static void safeCopy(char* dest, const char* src, int maxLen) {
    strncpy(dest, src, maxLen - 1);
    dest[maxLen - 1] = '\0';
}

// Portable case-insensitive string comparison (no POSIX dependency)
// Returns 0 if equal, negative if a < b, positive if a > b
static int strIcmp(const char* a, const char* b) {
    for (;;) {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb || ca == '\0') return ca - cb;
        ++a; ++b;
    }
}

// Print a horizontal separator line of given character and width
static void printSep(char ch = '-', int w = SEP_W) {
    for (int i = 0; i < w; i++) cout << ch;
    cout << '\n';
}


// ================================================================
//  STRUCT: Book
//  ----------------------------------------------------------------
//  Plain data holder for a single library book.
//  Contains the title, author, and current availability status.
// ================================================================
struct Book {
    char title[MAX_LEN];     // book title
    char author[MAX_LEN];    // author name(s)
    bool isAvailable;        // true = on shelf, false = checked out

    // Default constructor – marks book available, clears strings
    Book() : isAvailable(true) {
        title[0] = author[0] = '\0';
    }

    // Parameterised constructor – copies title/author safely
    Book(const char* t, const char* a) : isAvailable(true) {
        safeCopy(title,  t, MAX_LEN);
        safeCopy(author, a, MAX_LEN);
    }
};


// ================================================================
//  STRUCT: QueueNode
//  ----------------------------------------------------------------
//  One node in the student-waiting linked list.
//  Stores a single student's name and a pointer to the next node.
// ================================================================
struct QueueNode {
    char       studentName[MAX_LEN];   // student waiting for a book
    QueueNode* next;                   // link to the next waiting student

    explicit QueueNode(const char* name) : next(nullptr) {
        safeCopy(studentName, name, MAX_LEN);
    }
};


// ================================================================
//  CLASS: RequestQueue
//  ----------------------------------------------------------------
//  A FIFO queue implemented with a singly linked list.
//  One RequestQueue is EMBEDDED (value member) inside every
//  LibraryNode, so it is automatically constructed and destroyed
//  with its parent node – no dangling pointers, no leaks.
//
//  Invariants:
//    front == nullptr  <=>  rear == nullptr  <=>  count == 0
//    rear->next == nullptr  (always)
//
//  Time complexities:
//    enqueue  –  O(1)  insert at rear
//    dequeue  –  O(1)  remove from front
//    peek     –  O(1)  read front without removing
//    display  –  O(n)  iterate all nodes
// ================================================================
class RequestQueue {
private:
    QueueNode* front;   // oldest request (dequeue end)
    QueueNode* rear;    // newest request (enqueue end)
    int        count;   // number of nodes currently in queue

    // Internal helper: deallocates all nodes without resetting members
    void freeNodes() {
        while (front) {
            QueueNode* tmp = front;
            front = front->next;
            delete tmp;
        }
    }

public:
    // ── Constructor ───────────────────────────────────────────────
    RequestQueue() : front(nullptr), rear(nullptr), count(0) {}

    // ── Destructor ────────────────────────────────────────────────
    // Walks the queue and deletes every QueueNode to prevent leaks.
    ~RequestQueue() {
        freeNodes();
        // front / rear are dangling after this, but the object is gone
    }

    // ── Copy Constructor (deep copy) ──────────────────────────────
    // Required because LibraryNode contains RequestQueue by value.
    // Copies every node so both queues are fully independent.
    RequestQueue(const RequestQueue& other)
        : front(nullptr), rear(nullptr), count(0) {
        for (QueueNode* n = other.front; n; n = n->next)
            enqueue(n->studentName);
    }

    // ── Copy Assignment (deep copy) ───────────────────────────────
    RequestQueue& operator=(const RequestQueue& other) {
        if (this != &other) {
            freeNodes();
            front = rear = nullptr;
            count = 0;
            for (QueueNode* n = other.front; n; n = n->next)
                enqueue(n->studentName);
        }
        return *this;
    }

    // ── enqueue ───────────────────────────────────────────────────
    // Appends a new student name at the rear of the queue.
    // A new QueueNode is allocated and linked after the current rear.
    // Time Complexity: O(1)
    void enqueue(const char* studentName) {
        QueueNode* node = new QueueNode(studentName);
        if (!rear) {
            front = rear = node;          // first insertion: both pointers set
        } else {
            rear->next = node;            // link after current rear
            rear       = node;            // advance rear pointer
        }
        ++count;
    }

    // ── dequeue ───────────────────────────────────────────────────
    // Removes the front node and copies the student's name into outName.
    // Returns false (no-op) if the queue is empty.
    // Time Complexity: O(1)
    bool dequeue(char* outName) {
        if (!front) return false;
        safeCopy(outName, front->studentName, MAX_LEN);
        QueueNode* tmp = front;
        front = front->next;              // advance front pointer
        if (!front) rear = nullptr;       // queue is now empty
        delete tmp;
        --count;
        return true;
    }

    // ── peek ──────────────────────────────────────────────────────
    // Reads the front student's name WITHOUT removing the node.
    // Returns false if the queue is empty.
    // Time Complexity: O(1)
    bool peek(char* outName) const {
        if (!front) return false;
        safeCopy(outName, front->studentName, MAX_LEN);
        return true;
    }

    bool isEmpty()  const { return count == 0; }
    int  getCount() const { return count;       }

    // ── display ───────────────────────────────────────────────────
    // Iterates the queue from front to rear and prints each student
    // with their 1-based position in line.
    // Time Complexity: O(n)
    void display() const {
        if (!front) { cout << "      (none)\n"; return; }
        int pos = 1;
        for (QueueNode* n = front; n; n = n->next, ++pos)
            cout << "      " << pos << ". " << n->studentName << '\n';
    }
};


// ================================================================
//  STRUCT: LibraryNode
//  ----------------------------------------------------------------
//  One node in the Library's singly linked list (the catalogue).
//  Each node owns:
//    book         – metadata of the book
//    waitingQueue – FIFO list of students waiting for this book
//    next         – pointer to the next book node in the catalogue
//
//  Copying is disabled because LibraryNode owns raw heap resources
//  through waitingQueue, and there is no meaningful copy semantics.
// ================================================================
struct LibraryNode {
    Book         book;           // embedded Book object (title, author, status)
    RequestQueue waitingQueue;   // embedded queue; destructor auto-called
    LibraryNode* next;           // link to next book in catalogue

    LibraryNode(const char* title, const char* author)
        : book(title, author), next(nullptr) {}

    // Prevent accidental copying (Rule of Three compliance)
    LibraryNode(const LibraryNode&)            = delete;
    LibraryNode& operator=(const LibraryNode&) = delete;
};


// ================================================================
//  CLASS: Library
//  ----------------------------------------------------------------
//  Core engine of the portal.
//  Manages a singly linked list of LibraryNodes and exposes
//  all eight menu operations.
//
//  Private members:
//    head      – pointer to the first book node
//    bookCount – total number of books in the catalogue
// ================================================================
class Library {
private:
    LibraryNode* head;       // sentinel to first node in catalogue
    int          bookCount;  // number of books currently stored

    // ── findBook (private helper) ─────────────────────────────────
    // Linear search through the linked list for a node whose title
    // matches the query (case-insensitive).
    // Returns: pointer to found node, or nullptr if not found.
    // Time Complexity: O(n)
    LibraryNode* findBook(const char* title) const {
        for (LibraryNode* cur = head; cur; cur = cur->next)
            if (strIcmp(cur->book.title, title) == 0)
                return cur;
        return nullptr;
    }

    // ── printBookEntry (private helper) ──────────────────────────
    // Formats and prints all details of a single book node.
    // Used by displayAllBooks() and searchBook().
    void printBookEntry(const LibraryNode* node, int idx) const {
        cout << "  [" << idx << "]  Title   : " << node->book.title  << '\n'
             << "        Author  : " << node->book.author             << '\n'
             << "        Status  : "
             << (node->book.isAvailable ? "Available   [OK]"
                                        : "Checked Out [--]")        << '\n'
             << "        Waiting : " << node->waitingQueue.getCount()
             << " student(s)\n";
        printSep();
    }

    // Prevent copying – Library owns raw pointers; no copy semantics
    Library(const Library&)            = delete;
    Library& operator=(const Library&) = delete;

public:
    // ── Constructor ───────────────────────────────────────────────
    Library() : head(nullptr), bookCount(0) {}

    // ── Destructor ────────────────────────────────────────────────
    // Walks the entire linked list and deletes every LibraryNode.
    // Because waitingQueue is a value member inside LibraryNode,
    // ~RequestQueue() is invoked automatically for each node,
    // which in turn frees all QueueNode objects in the queue.
    // Net result: zero memory leaks.
    ~Library() {
        while (head) {
            LibraryNode* tmp = head;
            head = head->next;
            delete tmp;   // triggers ~Book() and ~RequestQueue()
        }
        bookCount = 0;
    }


    // ============================================================
    //  OPERATION 1 – addBook
    //  ----------------------------------------------------------
    //  Creates a new LibraryNode and appends it to the tail of the
    //  catalogue linked list.
    //
    //  Validation:
    //    • Rejects empty title or author strings.
    //    • Rejects duplicate titles (case-insensitive check).
    //
    //  Time Complexity: O(n)
    //    – one pass to check for duplicates (findBook)
    //    – one pass to find the tail for appending
    // ============================================================
    void addBook(const char* title, const char* author) {
        // --- Input validation ---
        if (!title  || title[0]  == '\0') {
            cout << "  [!] Title cannot be empty.\n"; return;
        }
        if (!author || author[0] == '\0') {
            cout << "  [!] Author cannot be empty.\n"; return;
        }

        // --- Duplicate check ---
        if (findBook(title)) {
            cout << "  [!] \"" << title
                 << "\" already exists in the catalogue.\n";
            return;
        }

        // --- Allocate new node ---
        LibraryNode* node = new LibraryNode(title, author);

        // --- Append to tail of linked list ---
        if (!head) {
            head = node;                        // first book ever added
        } else {
            LibraryNode* tail = head;
            while (tail->next) tail = tail->next;
            tail->next = node;
        }
        ++bookCount;
        cout << "  [+] \"" << title << "\" by "
             << author << " – added successfully.\n";
    }


    // ============================================================
    //  OPERATION 2 – deleteBook
    //  ----------------------------------------------------------
    //  Finds the node by title and unlinks it from the list.
    //
    //  Guards (all must pass):
    //    • Library is not empty.
    //    • Book with the given title exists.
    //    • Book is currently available (not checked out).
    //    • Book's waiting queue is empty (no pending requests).
    //
    //  Time Complexity: O(n)
    // ============================================================
    void deleteBook(const char* title) {
        if (!head) {
            cout << "  [!] Library is empty.\n"; return;
        }

        // Track previous node for unlinking
        LibraryNode* prev = nullptr;
        LibraryNode* cur  = head;

        // Walk until title matches or end of list
        while (cur && strIcmp(cur->book.title, title) != 0) {
            prev = cur;
            cur  = cur->next;
        }

        if (!cur) {
            cout << "  [!] \"" << title << "\" not found.\n"; return;
        }
        if (!cur->book.isAvailable) {
            cout << "  [!] Cannot delete \"" << title
                 << "\" – currently checked out.\n"; return;
        }
        if (!cur->waitingQueue.isEmpty()) {
            cout << "  [!] Cannot delete \"" << title << "\" – "
                 << cur->waitingQueue.getCount()
                 << " pending request(s) remain.\n"; return;
        }

        // --- Unlink node from the list ---
        if (!prev) head       = cur->next;   // deleting head
        else        prev->next = cur->next;  // deleting mid/tail

        delete cur;   // ~RequestQueue() and ~Book() called automatically
        --bookCount;
        cout << "  [-] \"" << title << "\" removed from the catalogue.\n";
    }


    // ============================================================
    //  OPERATION 3 – displayAllBooks
    //  ----------------------------------------------------------
    //  Traverses the entire linked list and pretty-prints every
    //  book's title, author, availability, and queue depth.
    //
    //  Time Complexity: O(n)
    // ============================================================
    void displayAllBooks() const {
        if (!head) {
            cout << "  [!] The library has no books yet.\n"; return;
        }
        cout << '\n';
        printSep('=');
        cout << "  CATALOGUE  –  " << bookCount
             << " book" << (bookCount != 1 ? "s" : "") << '\n';
        printSep('=');
        int idx = 1;
        for (LibraryNode* cur = head; cur; cur = cur->next, ++idx)
            printBookEntry(cur, idx);
    }


    // ============================================================
    //  OPERATION 4 – searchBook
    //  ----------------------------------------------------------
    //  Case-insensitive linear search by title.
    //  Prints full details of the matched book.
    //
    //  Time Complexity: O(n)
    // ============================================================
    void searchBook(const char* title) const {
        LibraryNode* node = findBook(title);
        if (!node) {
            cout << "  [!] \"" << title << "\" not found.\n"; return;
        }
        cout << '\n';
        printSep('=');
        cout << "  SEARCH RESULT\n";
        printSep('=');
        printBookEntry(node, 1);
    }


    // ============================================================
    //  OPERATION 5 – borrowBook
    //  ----------------------------------------------------------
    //  Borrow Logic:
    //    CASE A (book available):
    //      → Set isAvailable = false
    //      → Confirm issuance to the student
    //
    //    CASE B (book unavailable):
    //      → Enqueue student's name in that book's waiting queue
    //      → Confirm queue position
    //
    //  Time Complexity: O(n) for search + O(1) for enqueue
    // ============================================================
    void borrowBook(const char* title, const char* studentName) {
        if (!studentName || studentName[0] == '\0') {
            cout << "  [!] Student name cannot be empty.\n"; return;
        }
        LibraryNode* node = findBook(title);
        if (!node) {
            cout << "  [!] \"" << title << "\" not found.\n"; return;
        }

        if (node->book.isAvailable) {
            // --- CASE A: Issue directly ---
            node->book.isAvailable = false;
            cout << "  [>>] \"" << title
                 << "\" issued to " << studentName
                 << ". Enjoy reading!\n";
        } else {
            // --- CASE B: Add to waiting queue ---
            node->waitingQueue.enqueue(studentName);
            cout << "  [Q]  \"" << title
                 << "\" is currently checked out.\n"
                 << "       " << studentName
                 << " added to the waiting queue.\n"
                 << "       Queue position : #"
                 << node->waitingQueue.getCount() << '\n';
        }
    }


    // ============================================================
    //  OPERATION 6 – returnBook
    //  ----------------------------------------------------------
    //  Return Logic:
    //    CASE A (waiting queue is empty):
    //      → Set isAvailable = true (book goes back on shelf)
    //
    //    CASE B (students are waiting):
    //      → Dequeue the first student (FIFO)
    //      → Auto-issue the book to that student
    //      → isAvailable stays false (book is still checked out)
    //
    //  NOTE: We never set isAvailable = true then false again;
    //        the flag stays false throughout CASE B because the
    //        book is immediately handed to the next student.
    //
    //  Time Complexity: O(n) for search + O(1) for dequeue
    // ============================================================
    void returnBook(const char* title) {
        LibraryNode* node = findBook(title);
        if (!node) {
            cout << "  [!] \"" << title << "\" not found.\n"; return;
        }
        if (node->book.isAvailable) {
            cout << "  [!] \"" << title
                 << "\" is already on the shelf. Nothing to return.\n";
            return;
        }

        char nextStudent[MAX_LEN];

        if (node->waitingQueue.isEmpty()) {
            // --- CASE A: No one waiting – simply mark available ---
            node->book.isAvailable = true;
            cout << "  [OK] \"" << title
                 << "\" returned. Now available on the shelf.\n";
        } else {
            // --- CASE B: Auto-issue to next student in queue ---
            node->waitingQueue.dequeue(nextStudent);
            // isAvailable intentionally stays false
            cout << "  [OK] \"" << title << "\" returned.\n"
                 << "  [>>] Auto-issued to next in queue: "
                 << nextStudent << '\n';
            int rem = node->waitingQueue.getCount();
            if (rem > 0)
                cout << "       " << rem << " student"
                     << (rem != 1 ? "s" : "")
                     << " still waiting.\n";
            else
                cout << "       Waiting list is now empty.\n";
        }
    }


    // ============================================================
    //  OPERATION 7 – displayPendingRequests
    //  ----------------------------------------------------------
    //  Iterates every LibraryNode.  For each book whose waiting
    //  queue is non-empty, prints the queue contents.
    //
    //  Time Complexity: O(n * m)
    //    n = number of books in catalogue
    //    m = average depth of each waiting queue
    // ============================================================
    void displayPendingRequests() const {
        if (!head) {
            cout << "  [!] Library is empty.\n"; return;
        }
        cout << '\n';
        printSep('=');
        cout << "  PENDING REQUEST REPORT\n";
        printSep('=');

        bool anyFound = false;
        for (LibraryNode* cur = head; cur; cur = cur->next) {
            if (!cur->waitingQueue.isEmpty()) {
                anyFound = true;
                cout << "  Book   : " << cur->book.title  << '\n'
                     << "  Author : " << cur->book.author << '\n'
                     << "  Queue  : "
                     << cur->waitingQueue.getCount()
                     << " student(s) waiting\n";
                cur->waitingQueue.display();
                printSep();
            }
        }
        if (!anyFound)
            cout << "  [OK] No pending requests at this time.\n";
    }

    int getBookCount() const { return bookCount; }
};


// ================================================================
//  INPUT HELPER: readLine
//  ----------------------------------------------------------------
//  Reads a trimmed, non-empty line from stdin into buf.
//  If 'required' is true, re-prompts the user on empty input.
//  Handles leading whitespace trimming in-place.
// ================================================================
static void readLine(const char* prompt, char* buf, int maxLen,
                     bool required = true) {
    for (;;) {
        cout << prompt;
        if (!cin.getline(buf, maxLen)) {
            cin.clear();
            buf[0] = '\0';
        }

        // Trim leading whitespace (spaces and tabs)
        int s = 0;
        while (buf[s] == ' ' || buf[s] == '\t') ++s;
        if (s > 0) {
            int i = 0;
            while ((buf[i] = buf[s + i]) != '\0') ++i;
        }

        if (buf[0] != '\0' || !required) return;
        cout << "  [!] This field cannot be empty. Please try again.\n";
    }
}

// ================================================================
//  INPUT HELPER: readChoice
//  ----------------------------------------------------------------
//  Reads and validates an integer menu choice in [lo, hi].
//  Rejects non-numeric input and out-of-range values.
// ================================================================
static int readChoice(int lo, int hi) {
    char buf[16];
    for (;;) {
        cout << "\n  Choice [" << lo << '-' << hi << "]: ";
        if (!cin.getline(buf, 16)) { cin.clear(); continue; }

        // Reject empty or non-digit input
        bool ok = (buf[0] != '\0');
        for (int i = 0; buf[i] && ok; ++i)
            ok = (isdigit((unsigned char)buf[i]) != 0);

        if (ok) {
            int v = atoi(buf);
            if (v >= lo && v <= hi) return v;
        }
        cout << "  [!] Please enter a number between "
             << lo << " and " << hi << ".\n";
    }
}


// ================================================================
//  MENU DISPLAY
// ================================================================
static void showMenu() {
    cout << "\n"
            "  +--------------------------------------------+\n"
            "  |        DIGITAL  LIBRARY  PORTAL            |\n"
            "  +--------------------------------------------+\n"
            "  |  1.  Add Book                              |\n"
            "  |  2.  Delete Book                           |\n"
            "  |  3.  Display All Books                     |\n"
            "  |  4.  Search Book by Title                  |\n"
            "  |  5.  Borrow Book                           |\n"
            "  |  6.  Return Book                           |\n"
            "  |  7.  Display Pending Requests              |\n"
            "  |  8.  Exit                                  |\n"
            "  +--------------------------------------------+\n";
}


// ================================================================
//  MAIN
//  ----------------------------------------------------------------
//  Entry point.
//    1. Constructs the Library object on the stack.
//    2. Seeds five sample books.
//    3. Runs the menu event loop until the user chooses Exit.
//    4. On scope exit, ~Library() deallocates ALL dynamic memory
//       automatically – no explicit cleanup needed in main().
// ================================================================
int main() {
    Library lib;
    char title[MAX_LEN], author[MAX_LEN], student[MAX_LEN];

    // ── Pre-load sample books (silently skipped if duplicate) ─────
    lib.addBook("The Great Gatsby",            "F. Scott Fitzgerald");
    lib.addBook("To Kill a Mockingbird",       "Harper Lee");
    lib.addBook("1984",                        "George Orwell");
    lib.addBook("Clean Code",                  "Robert C. Martin");
    lib.addBook("Introduction to Algorithms",  "Thomas H. Cormen");

    cout << '\n';
    printSep('=');
    cout << "    Welcome to the Digital Library Portal\n"
         << "    " << lib.getBookCount() << " books pre-loaded.\n";
    printSep('=');

    bool running = true;
    while (running) {
        showMenu();
        int ch = readChoice(1, 8);
        cout << '\n';
        printSep('=');

        switch (ch) {

        case 1:   // ── Add Book ──────────────────────────────────
            readLine("  Title  : ", title,   MAX_LEN);
            readLine("  Author : ", author,  MAX_LEN);
            lib.addBook(title, author);
            break;

        case 2:   // ── Delete Book ───────────────────────────────
            readLine("  Title to delete : ", title, MAX_LEN);
            lib.deleteBook(title);
            break;

        case 3:   // ── Display All Books ─────────────────────────
            lib.displayAllBooks();
            break;

        case 4:   // ── Search Book by Title ──────────────────────
            readLine("  Title to search : ", title, MAX_LEN);
            lib.searchBook(title);
            break;

        case 5:   // ── Borrow Book ───────────────────────────────
            readLine("  Book title   : ", title,   MAX_LEN);
            readLine("  Student name : ", student, MAX_LEN);
            lib.borrowBook(title, student);
            break;

        case 6:   // ── Return Book ───────────────────────────────
            readLine("  Title to return : ", title, MAX_LEN);
            lib.returnBook(title);
            break;

        case 7:   // ── Display Pending Requests ──────────────────
            lib.displayPendingRequests();
            break;

        case 8:   // ── Exit ──────────────────────────────────────
            cout << "  Thank you for using the Digital Library Portal.\n"
                 << "  Goodbye!\n";
            running = false;
            break;
        }
        printSep('=');
    }

    return 0;
    // ── ~Library() called here automatically ──────────────────────
    // Deletes every LibraryNode in the catalogue linked list.
    // For each node, ~RequestQueue() deletes every QueueNode.
    // Zero manual cleanup required in main(). This is RAII.
}
