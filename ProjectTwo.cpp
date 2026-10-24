
/*
 * CS 300 - Project Two
 * ABCU Advising Assistance Program
 * Author: Saul Osorio
 *
 * This program uses a Binary Search Tree
 * to store and search university courses.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;

// Structure to hold information about one course
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Structure for each node in the Binary Search Tree
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


// Binary Search Tree class
class BinarySearchTree {
private:
    Node* root;


    // Recursively release memory used by the BST

    void destroyTree(Node* node) {

        if (node == nullptr) {

            return;

        }



        destroyTree(node->left);

        destroyTree(node->right);



        delete node;

    }



    // Search for a course by its course number
    Node* searchNode(Node* node, const string& courseNumber) {
        while (node != nullptr) {
            if (courseNumber == node->course.courseNumber) {
                return node;
            }
            else if (courseNumber < node->course.courseNumber) {
                node = node->left;
            }
            else {
                node = node->right;
            }
        }

        return nullptr;
    }


    // Print courses in alphanumeric order
    void inOrder(Node* node) {
        if (node == nullptr) {
            return;
        }

        // Visit the left subtree first
        inOrder(node->left);

        // Display the course number and title
        cout << node->course.courseNumber << ", "
            << node->course.courseTitle << endl;

        // Visit the right subtree
        inOrder(node->right);
    }


    // Recursively insert a course into the correct position
    Node* insertNode(Node* node, const Course& course) {
        if (node == nullptr) {
            return new Node(course);
        }

        if (course.courseNumber < node->course.courseNumber) {
            node->left = insertNode(node->left, course);
        }
        else if (course.courseNumber > node->course.courseNumber) {
            node->right = insertNode(node->right, course);
        }
        else {
            // Update the course if its number already exists
            node->course = course;
        }

        return node;
    }

public:
    BinarySearchTree() {
        root = nullptr;
    }

    // Destructor to prevent memory leaks
    ~BinarySearchTree() {
        destroyTree(root);
    }


    // Remove all courses from the tree

    void Clear() {

        destroyTree(root);

        root = nullptr;

    }



    // Public function to insert a course
    void Insert(const Course& course) {
        root = insertNode(root, course);
    }

    // Display all courses in sorted order
    void PrintAllCourses() {
        cout << endl;
        cout << "Here is a sample schedule:" << endl;
        cout << endl;

        inOrder(root);
    }

    // Find a course and display its information
    void PrintCourseInformation(string courseNumber) {

        // Convert lowercase input to uppercase
        transform(courseNumber.begin(), courseNumber.end(),
            courseNumber.begin(),
            [](unsigned char c) { return toupper(c); });

        Node* found = searchNode(root, courseNumber);

        if (found == nullptr) {
            cout << "Course not found." << endl;
            return;
        }

        cout << found->course.courseNumber << ", "
            << found->course.courseTitle << endl;

        if (found->course.prerequisites.empty()) {
            cout << "Prerequisites: None" << endl;
            return;
        }

        cout << "Prerequisites: ";

        for (size_t i = 0; i < found->course.prerequisites.size(); i++) {
            string prereqNumber = found->course.prerequisites[i];

            Node* prereq = searchNode(root, prereqNumber);

            cout << prereqNumber;

            if (prereq != nullptr) {
                cout << ", " << prereq->course.courseTitle;
            }

            if (i + 1 < found->course.prerequisites.size()) {
                cout << " | ";
            }
        }

        cout << endl;
    }
};



// Remove spaces from the beginning and end of a string
string trim(const string& text) {
    size_t first = text.find_first_not_of(" \t\r\n");

    if (first == string::npos) {
        return "";
    }

    size_t last = text.find_last_not_of(" \t\r\n");

    return text.substr(first, last - first + 1);
}

// Load and validate course data from a CSV file
bool loadCourses(const string& fileName, BinarySearchTree& courses) {
    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Error: Could not open file " << fileName << endl;
        return false;
    }

    vector<Course> courseList;
    string line;

    // Read each course from the file
    while (getline(file, line)) {
        if (trim(line).empty()) {
            continue;
        }

        stringstream ss(line);
        string token;
        vector<string> fields;

        while (getline(ss, token, ',')) {
            fields.push_back(trim(token));
        }

        // Every course needs a number and title
        if (fields.size() < 2 ||
            fields[0].empty() || fields[1].empty()) {
            cout << "Error: Invalid course information." << endl;
            return false;
        }

        Course course;
        course.courseNumber = fields[0];
        course.courseTitle = fields[1];

        // Remaining fields contain prerequisites
        for (size_t i = 2; i < fields.size(); i++) {
            if (!fields[i].empty()) {
                course.prerequisites.push_back(fields[i]);
            }
        }

        courseList.push_back(course);
    }

    file.close();

    // Check that every prerequisite exists in the file
    for (const Course& course : courseList) {
        for (const string& prereq : course.prerequisites) {
            bool found = false;

            for (const Course& other : courseList) {
                if (other.courseNumber == prereq) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                cout << "Error: Prerequisite " << prereq
                    << " does not exist." << endl;
                return false;
            }
        }
    }


    // Remove previously loaded courses
    courses.Clear();

    // Store all validated courses in the BST
    for (const Course& course : courseList) {
        courses.Insert(course);
    }


    cout << courseList.size()
        << " courses loaded successfully." << endl;

    return true;
}




/**

 * Main program for ABCU Advising Assistance

 * Displays the menu and processes user selections.

 */

int main() {

    BinarySearchTree courses;

    bool dataLoaded = false;

    int choice = 0;

    string fileName;

    string courseNumber;



    cout << "Welcome to the course planner." << endl;



    while (choice != 9) {

        cout << endl;

        cout << "    1. Load Data Structure." << endl;

        cout << "    2. Print Course List." << endl;

        cout << "    3. Print Course." << endl;

        cout << "    9. Exit" << endl;

        cout << endl;

        cout << "What would you like to do? ";



        // Validate menu input

        if (!(cin >> choice)) {

            cin.clear();

            cin.ignore(10000, '\n');

            cout << "Invalid input. Please enter a number." << endl;

            continue;

        }



        switch (choice) {

        case 1:

            cout << "Enter the course file name: ";

            cin >> ws;

            getline(cin, fileName);



            // Load the CSV file into the BST


            // Keep existing data if the new file fails to load
            if (loadCourses(fileName, courses)) {
                dataLoaded = true;
            }


            break;



        case 2:

            if (!dataLoaded) {

                cout << "Please load the course file first." << endl;

            }

            else {

                courses.PrintAllCourses();

            }

            break;



        case 3:

            if (!dataLoaded) {

                cout << "Please load the course file first." << endl;

            }

            else {

                cout << "What course do you want to know about? ";

                cin >> courseNumber;



                cout << endl;

                courses.PrintCourseInformation(courseNumber);

            }

            break;



        case 9:

            cout << "Thank you for using the course planner!" << endl;

            break;



        default:

            cout << choice << " is not a valid option." << endl;

            break;

        }

    }



    return 0;

}


