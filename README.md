# Digital Library Portal

A C++ based Digital Library Management System developed using Object-Oriented Programming, Singly Linked Lists, and Queue data structures. The system automates book management, borrowing, returning, and request handling while maintaining a FIFO-based waiting list for efficient book allocation.

---

## Features

- Add, search, display, and delete books from the library catalogue.
- Borrow and return books with real-time availability tracking.
- Automatic book allocation to the next student using a FIFO waiting queue.
- Maintain pending requests for unavailable books.
- Prevent duplicate book entries and invalid operations.
- Case-insensitive book search for improved usability.
- Dynamic memory management using `new` and `delete` without STL containers.

---

## Data Structures Used

- Singly Linked List
- Queue (Linked List Implementation)
- Custom Nodes
- Dynamic Memory Allocation

---

## Object-Oriented Concepts

- Classes and Objects
- Encapsulation
- Constructors and Destructors
- Deep Copy Constructor
- Copy Assignment Operator
- RAII (Resource Acquisition Is Initialization)

---

## Project Workflow

1. Add new books to the library catalogue.
2. Search books using their title.
3. Borrow available books instantly.
4. If unavailable, automatically add students to the waiting queue.
5. Return borrowed books.
6. Automatically issue returned books to the next waiting student.
7. Display complete catalogue and pending request lists.

---

## Functionalities

### Add Book
- Adds a new book to the catalogue.
- Prevents duplicate entries.

### Delete Book
- Deletes books only if they are available and have no pending requests.

### Search Book
- Performs case-insensitive title search.

### Borrow Book
- Issues available books immediately.
- Adds students to a FIFO waiting queue if the book is unavailable.

### Return Book
- Returns books to the library.
- Automatically allocates returned books to the first student in the waiting queue.

### Display Catalogue
- Shows complete book details including:
  - Title
  - Author
  - Availability Status
  - Number of Pending Requests

### Display Pending Requests
- Displays waiting lists for every unavailable book.

---

## Time Complexity

| Operation | Complexity |
|-----------|------------|
| Add Book | O(n) |
| Delete Book | O(n) |
| Search Book | O(n) |
| Borrow Book | O(n) |
| Return Book | O(n) |
| Queue Enqueue | O(1) |
| Queue Dequeue | O(1) |

---

## Technologies Used

- C++
- Object-Oriented Programming
- Singly Linked List
- Queue
- Dynamic Memory Allocation
- Manual Memory Management

---

## Learning Outcomes

- Implementation of custom Linked Lists without STL
- Queue implementation using Linked Lists
- Dynamic memory allocation and deallocation
- Deep copy and Rule of Three
- FIFO scheduling
- Resource management using RAII
- Object-Oriented software design

---

## Future Enhancements

- User authentication and role management
- Book issue history
- Fine calculation for overdue books
- File-based database persistence
- ISBN-based search
- Multiple book copies
- Graphical User Interface
- Database integration using MySQL

---

## Author

**Rehan Khan**

B.Tech, Civil Engineering  
Indian Institute of Technology Kharagpur
