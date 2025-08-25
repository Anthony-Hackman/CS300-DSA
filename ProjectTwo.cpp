// ProjectTwo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// Author: Anthony Hackman
// Date: 2025-08-16

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <set>

// STRUCTURES
struct Course
{
    std::string courseNumber;
    std::string title;
    std::vector<std::string> prerequisites;
};

// GLOBAL VARIABLES
std::vector<Course> vecCourses;

// HELPER FUNCTIONS
// @brief Helper functions for string manipulation and validation
//
// Helper: trim whitespace from both ends
std::string trim(const std::string &s)
{
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start))
        ++start;
    auto end = s.end();
    if (start == end)
        return ""; // string is all whitespace or empty
    do
    {
        --end;
    } while (end != start && std::isspace(*end));
    return std::string(start, end + 1);
}
//
// Helper: split CSV line
std::vector<std::string> splitCSV(const std::string &line)
{
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, ','))
    {
        tokens.push_back(trim(item));
    }
    return tokens;
}
//
// Helper: validate prerequisites
bool validatePrerequisites(const std::vector<Course> &courses)
{
    std::set<std::string> ids;
    for (const auto &c : courses)
        ids.insert(c.courseNumber);
    for (const auto &c : courses)
    {
        if (std::find(c.prerequisites.begin(), c.prerequisites.end(), c.courseNumber) != c.prerequisites.end())
        {
            std::cout << "Self prerequisite: " << c.courseNumber << std::endl;
            return false;
        }
        std::set<std::string> seen;
        for (const auto &p : c.prerequisites)
        {
            if (!seen.insert(p).second)
            {
                std::cout << "Duplicate prerequisite " << p << " for " << c.courseNumber << std::endl;
                return false;
            }
            if (ids.find(p) == ids.end())
            {
                std::cout << "Missing course for prerequisite " << p << " (in " << c.courseNumber << ")" << std::endl;
                return false;
            }
        }
    }
    return true;
}
//
// Helper: case-insensitive string compare
int strcasecmp(const char *s1, const char *s2)
{
    while (*s1 && *s2)
    {
        char c1 = std::tolower(static_cast<unsigned char>(*s1));
        char c2 = std::tolower(static_cast<unsigned char>(*s2));
        if (c1 != c2)
            return c1 - c2;
        ++s1;
        ++s2;
    }
    return std::tolower(static_cast<unsigned char>(*s1)) - std::tolower(static_cast<unsigned char>(*s2));
}

// FUNCTION DEFINITIONS
// @brief Function definitions for loading data, printing course info, and main menu
//
// Load courses from file
std::vector<Course> LoadCoursesFromFile(const std::string &path)
{
    std::ifstream file(path);
    if (!file)
    {
        std::cout << "File error" << std::endl;
        return {};
    }
    std::vector<Course> courses;
    std::string line;
    int lineNo = 0;
    while (std::getline(file, line))
    {
        ++lineNo;
        auto parts = splitCSV(line);
        if (parts.size() < 2)
        {
            std::cout << "Line " << lineNo << " missing id or title" << std::endl;
            continue;
        }
        Course c;
        c.courseNumber = parts[0];
        c.title = parts[1];
        for (size_t i = 2; i < parts.size(); ++i)
        {
            if (!parts[i].empty())
                c.prerequisites.push_back(parts[i]);
        }
        courses.push_back(c);
    }
    if (!validatePrerequisites(courses))
        return {};
    return courses;
}

// Load data into vector
bool LoadData_Vector(const std::string &path)
{
    vecCourses = LoadCoursesFromFile(path);
    return !vecCourses.empty();
}

// Print course info
void PrintCourseInfo_Vector(const std::string &target)
{
    for (const auto &c : vecCourses)
    {
        if (strcasecmp(c.courseNumber.c_str(), target.c_str()) == 0)
        {
            std::cout << c.courseNumber << ": " << c.title << std::endl;
            if (c.prerequisites.empty())
                std::cout << "Prerequisites: none" << std::endl;
            else
            {
                std::cout << "Prerequisites: ";
                for (size_t i = 0; i < c.prerequisites.size(); ++i)
                {
                    std::cout << c.prerequisites[i];
                    if (i + 1 < c.prerequisites.size())
                        std::cout << ", ";
                }
                std::cout << std::endl;
            }
            return;
        }
    }
    std::cout << "Course not found." << std::endl;
}

// Print course list (alphanumeric)
void PrintCourseList_Vector()
{
    std::vector<Course> sorted = vecCourses;
    std::sort(sorted.begin(), sorted.end(), [](const Course &a, const Course &b)
              { return a.courseNumber < b.courseNumber; });
    for (const auto &c : sorted)
    {
        std::cout << c.courseNumber << ": " << c.title << std::endl;
    }
}

// Main menu
void MainMenu()
{
    bool loaded = false;
    while (true)
    {
        std::cout << "1. Load Data" << std::endl;
        std::cout << "2. Print Course List (alphanumeric)" << std::endl;
        std::cout << "3. Print Course Info" << std::endl;
        std::cout << "9. Exit" << std::endl;
        std::cout << "Enter choice: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        if (choice == 1)
        {
            std::string path;
            do
            {
                std::cout << "Enter file path: ";
                std::getline(std::cin, path);
                if (path.empty())
                {
                    std::cout << "File path cannot be empty. Please try again." << std::endl;
                    continue;
                }
                loaded = LoadData_Vector(path);
                if (!loaded)
                {
                    std::cout << "Failed to load data from '" << path << "'. Please check the file and try again." << std::endl;
                }
            } while (!loaded);
        }
        else if (choice == 2)
        {
            if (!loaded)
                std::cout << "Load data first." << std::endl;
            else
                PrintCourseList_Vector();
        }
        else if (choice == 3)
        {
            if (!loaded)
                std::cout << "Load data first." << std::endl;
            else
            {
                std::string target;
                std::cout << "Enter course number: ";
                std::getline(std::cin, target);
                PrintCourseInfo_Vector(target);
            }
        }
        else if (choice == 9)
        {
            break;
        }
        else
        {
            std::cout << "Invalid selection." << std::endl;
        }
    }
}
// END OF FUNCTION DEFINITIONS

// Main
int main()
{
    MainMenu();
    return 0;
}