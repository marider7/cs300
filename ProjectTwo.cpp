#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>

using namespace std;


// ------------------------------------------------------------
// Course Structure
// Stores all information belonging to one course.
// ------------------------------------------------------------
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};


// ------------------------------------------------------------
// Binary Search Tree Node
// Each node stores one Course and pointers to its children.
// ------------------------------------------------------------
struct Node {
    Course course;
    Node* left;
    Node* right;

    Node(Course newCourse) {
        course = newCourse;
        left = nullptr;
        right = nullptr;
    }
};


// ------------------------------------------------------------
// Insert Course into Binary Search Tree
// Courses are organized by course number so that an
// in-order traversal will produce alphanumeric order.
// ------------------------------------------------------------
Node* insertCourse(Node* node, Course course) {

    // If this position is empty, create a new node.
    if (node == nullptr) {
        return new Node(course);
    }

    // Smaller course numbers are placed in the left subtree.
    if (course.courseNumber < node->course.courseNumber) {
        node->left = insertCourse(node->left, course);
    }

    // Larger course numbers are placed in the right subtree.
    else if (course.courseNumber > node->course.courseNumber) {
        node->right = insertCourse(node->right, course);
    }

    // Duplicate course numbers are not inserted.
    else {
        cout << "Duplicate course number found: "
             << course.courseNumber << endl;
    }

    return node;
}


// ------------------------------------------------------------
// Search for a Course
// The BST ordering allows the search to move left or right
// instead of checking every course.
// ------------------------------------------------------------
Node* searchCourse(Node* node, string courseNumber) {

    // Stop when the course is found or there are no more nodes.
    if (node == nullptr ||
        node->course.courseNumber == courseNumber) {
        return node;
    }

    // Search the left subtree when the requested course
    // number is smaller than the current course number.
    if (courseNumber < node->course.courseNumber) {
        return searchCourse(node->left, courseNumber);
    }

    // Otherwise, search the right subtree.
    return searchCourse(node->right, courseNumber);
}


// ------------------------------------------------------------
// Print Course List
// In-order traversal prints the BST from lowest to highest
// course number, satisfying the alphanumeric list requirement.
// ------------------------------------------------------------
void printCourseList(Node* node) {

    if (node == nullptr) {
        return;
    }

    // Visit smaller course numbers first.
    printCourseList(node->left);

    cout << node->course.courseNumber
         << " | "
         << node->course.courseTitle << endl;

    // Then visit larger course numbers.
    printCourseList(node->right);
}


// ------------------------------------------------------------
// Check Whether a Course Exists
// Used when validating prerequisites.
// ------------------------------------------------------------
bool courseExists(Node* root, string courseNumber) {
    return searchCourse(root, courseNumber) != nullptr;
}


// ------------------------------------------------------------
// Validate Prerequisites
// Every prerequisite listed in the input file must also
// exist as a course in the BST.
// ------------------------------------------------------------
bool validatePrerequisites(Node* node, Node* root) {

    if (node == nullptr) {
        return true;
    }

    // Validate prerequisites in the left subtree.
    if (!validatePrerequisites(node->left, root)) {
        return false;
    }

    // Check every prerequisite for the current course.
    for (string prerequisite : node->course.prerequisites) {

        if (!courseExists(root, prerequisite)) {
            cout << "Invalid prerequisite: "
                 << prerequisite
                 << " for course "
                 << node->course.courseNumber
                 << endl;

            return false;
        }
    }

    // Validate prerequisites in the right subtree.
    if (!validatePrerequisites(node->right, root)) {
        return false;
    }

    return true;
}


// ------------------------------------------------------------
// Load Courses from File
// Reads each line, separates the data, creates a Course
// object, and stores it in the Binary Search Tree.
// ------------------------------------------------------------
bool loadCourses(string fileName, Node*& root) {

    ifstream file(fileName.c_str());

    // The program cannot load courses if the file cannot
    // be opened, so report the error and return to the menu.
    if (!file.is_open()) {
        cout << "Make sure the file is in the program's working directory." << endl;
        return false;
    }
    cout << "file opened successfully!" << endl;

    string line;

    while (getline(file, line)) {

        // Break the current line into individual fields
        // using the comma as the delimiter.
        stringstream ss(line);
        string token;
        vector<string> tokens;

        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        // Every valid course must contain at least a course
        // number and course title. Prerequisites are optional.
        if (tokens.size() < 2) {
            cout << "Invalid file format. "
                 << "Each course must contain a course number "
                 << "and course title." << endl;
            continue;
        }

        Course course;

        // The first field contains the course number.
        course.courseNumber = tokens[0];

        // The second field contains the course title.
        course.courseTitle = tokens[1];

        // Any remaining fields are prerequisite course numbers.
        for (size_t i = 2; i < tokens.size(); i++) {
        	// Only store non-empty values as prerequisites.
        	if (!tokens[i].empty()) {
            course.prerequisites.push_back(tokens[i]);
        }
      }

        // Store the completed Course object in the BST.
        root = insertCourse(root, course);
    }

    file.close();

    // Validate prerequisites only after all courses have been
    // loaded so that every possible prerequisite can be found.
    if (!validatePrerequisites(root, root)) {
        cout << "Course data contains invalid prerequisites."
             << endl;
        return false;
    }

    cout << "Course data loaded successfully." << endl;

    return true;
}


// ------------------------------------------------------------
// Print Individual Course Information
// Displays the course title and both prerequisite numbers
// and titles, as required by Project Two.
// ------------------------------------------------------------
void printCourseInformation(Node* root, string courseNumber) {

    Node* courseNode = searchCourse(root, courseNumber);

    if (courseNode == nullptr) {
        cout << "Course not found." << endl;
        return;
    }

    Course course = courseNode->course;

    cout << endl;
    cout << "Course Number: " << course.courseNumber << endl;
    cout << "Course Title: " << course.courseTitle << endl;

    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    }
    else {
        cout << "Prerequisites:" << endl;

        // Look up each prerequisite so that both its course
        // number and title can be displayed.
        for (string prerequisite : course.prerequisites) {

            Node* prerequisiteNode =
                searchCourse(root, prerequisite);

            if (prerequisiteNode != nullptr) {
                cout << "  "
                     << prerequisiteNode->course.courseNumber
                     << " | "
                     << prerequisiteNode->course.courseTitle
                     << endl;
            }
        }
    }
}


// ------------------------------------------------------------
// Delete the Binary Search Tree
// Frees the memory used by all dynamically created nodes
// before the program ends.
// ------------------------------------------------------------
void deleteTree(Node* node) {

    if (node == nullptr) {
        return;
    }

    deleteTree(node->left);
    deleteTree(node->right);

    delete node;
}


// ------------------------------------------------------------
// Main Program
// Provides the required menu and controls the program flow.
// ------------------------------------------------------------
int main() {

    Node* root = nullptr;

    int choice = 0;
    bool dataLoaded = false;

    cout << "Welcome to the ABCU Computer Science "
         << "Course Advising System" << endl;

    while (choice != 9) {

        cout << endl;
        cout << "1. Load Data Structure" << endl;
        cout << "2. Print Course List" << endl;
        cout << "3. Print Course" << endl;
        cout << "9. Exit" << endl;
        cout << "Enter your choice: ";

        cin >> choice;

        // Handle invalid menu input such as letters instead of numbers.
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');

            cout << "Invalid menu option. "
                 << "Please enter 1, 2, 3, or 9."
                 << endl;

            continue;
        }

        if (choice == 1) {

            string fileName;
            cout << "Enter the course data file name: ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, fileName);


            // Clear any previously loaded data before loading
            // a new file so that old courses are not mixed with
            // the new course data.
            if (root != nullptr) {
                deleteTree(root);
                root = nullptr;
            }

            dataLoaded = loadCourses(fileName, root);

        }
        else if (choice == 2) {

            // Courses must be loaded before the program can
            // produce the requested course list.
            if (!dataLoaded) {
                cout << "Please load the course data first."
                     << endl;
            }
            else {
                cout << endl;
                cout << "Computer Science Course List"
                     << endl;
                cout << "-----------------------------"
                     << endl;

                printCourseList(root);
            }

        }
        else if (choice == 3) {

            if (!dataLoaded) {
                cout << "Please load the course data first."
                     << endl;
            }
            else {

                string courseNumber;

                cout << "Enter course number: ";
                cin >> courseNumber;

                printCourseInformation(root, courseNumber);
            }

        }
        else if (choice == 9) {

            cout << "Goodbye!" << endl;

        }
        else {

            cout << "Invalid menu option. "
                 << "Please enter 1, 2, 3, or 9."
                 << endl;
        }
    }

    // Release all dynamically allocated tree nodes before exiting.
    deleteTree(root);

    return 0;
}
