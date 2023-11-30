#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>
#include <string>
#include <set>
#include <fstream>
#include <sstream>
using namespace std;

struct Faculty
{
    string category;
    int faculty_number;         // Category of the faculty (x1, x2, x3)
    vector<string> preferences; // List of preferred courses
    vector<string> faculty_courses;
};

struct BipartiteGraph
{
    vector<Faculty> faculties;
    vector<string> courses;
};

bool bfs(vector<vector<int>> &adjMatrix, unordered_map<int, int> &pairU, unordered_map<int, int> &pairV, unordered_map<int, int> &dist)
{
    queue<int> q;

    for (auto u : pairU)
    {
        if (u.second == -1)
        {
            dist[u.first] = 0;
            q.push(u.first);
        }
        else
        {
            dist[u.first] = numeric_limits<int>::max();
        }
    }

    dist[-1] = numeric_limits<int>::max();

    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        if (dist[u] < dist[-1])
        {
            for (int v = 0; v < adjMatrix[u].size(); ++v)
            {
                if (adjMatrix[u][v] && dist[pairV[v]] == numeric_limits<int>::max())
                {
                    dist[pairV[v]] = dist[u] + 1;
                    q.push(pairV[v]);
                }
            }
        }
    }

    return (dist[-1] != numeric_limits<int>::max());
}

bool dfs(int u, vector<vector<int>> &adjMatrix, unordered_map<int, int> &pairU, unordered_map<int, int> &pairV, unordered_map<int, int> &dist)
{
    if (u != -1)
    {
        for (int v = 0; v < adjMatrix[u].size(); ++v)
        {
            if (adjMatrix[u][v] && dist[pairV[v]] == dist[u] + 1)
            {
                if (dfs(pairV[v], adjMatrix, pairU, pairV, dist))
                {
                    pairV[v] = u;
                    pairU[u] = v;
                    return true;
                }
            }
        }

        dist[u] = numeric_limits<int>::max();
        return false;
    }

    return true;
}

int hopcroftKarp(BipartiteGraph &graph, unordered_map<int, int> &facultyAssignments)
{
    vector<vector<int>> adjMatrix(graph.faculties.size(), vector<int>(graph.courses.size(), 0));

    for (int i = 0; i < graph.faculties.size(); ++i)
    {
        for (const string &course : graph.faculties[i].preferences)
        {
            auto courseIt = find(graph.courses.begin(), graph.courses.end(), course);
            if (courseIt != graph.courses.end())
            {
                int courseIndex = distance(graph.courses.begin(), courseIt);
                adjMatrix[i][courseIndex] = 1;
            }
        }
    }

    unordered_map<int, int> pairU;
    unordered_map<int, int> pairV;
    unordered_map<int, int> dist;

    for (int i = 0; i < graph.faculties.size(); ++i)
    {
        pairU[i] = -1;
    }

    for (int i = 0; i < graph.courses.size(); ++i)
    {
        pairV[i] = -1;
    }

    int matching = 0;

    while (bfs(adjMatrix, pairU, pairV, dist))
    {
        for (int i = 0; i < graph.faculties.size(); ++i)
        {
            if (pairU[i] == -1 && dfs(i, adjMatrix, pairU, pairV, dist))
            {
                matching++;
            }
        }
    }

    // Store faculty assignments
    for (int i = 0; i < graph.faculties.size(); ++i)
    {
        if (pairU[i] != -1)
        {
            facultyAssignments[i] = pairU[i];
        }
    }
    return matching;
}

vector<string> getUnassignedCourses(const vector<string> &course_list, const unordered_map<int, int> &facultyAssignments)
{
    vector<string> unassigned_course_list;
    unassigned_course_list.clear();
    set<string> course_set(course_list.begin(), course_list.end());
    vector<string> course_list_set(course_set.begin(), course_set.end());
    for (const string &course : course_list_set)
    {
        bool assigned = false;
        int occurrences = 0;

        for (const auto &assignment : facultyAssignments)
        {
            string course_number = to_string(assignment.second + 1);
            if ("C" + course_number == course)
            {
                occurrences++;
            }
        }

        if (occurrences == 0)
        {
            // Add it twice to unassigned course
            unassigned_course_list.push_back(course);
            unassigned_course_list.push_back(course);
        }
        else if (occurrences == 1 && !assigned)
        {
            // Add it once to unassigned course
            unassigned_course_list.push_back(course);
            assigned = true;
        }
    }

    return unassigned_course_list;
}
// Helps to rearrange duplicates to make sure second half of course is definitely allotted
vector<string> rearrangeDuplicates(const vector<string> &input)
{
    vector<string> result;

    for (const auto &item : input)
    {
        auto it = find(result.begin(), result.end(), item);

        if (it == result.end())
        {
            result.push_back(item);
        }
        else
        {
            result.insert(it, item);
        }
    }

    return result;
}

vector<string> removeDuplicateMembers(const vector<string> &input)
{
    vector<string> result;

    for (auto it = input.begin(); it != input.end(); it++)
    {
        auto next = std::next(it);
        if (next != input.end() && *it == *next)
        {
            it++;
        }
        else
        {
            result.push_back(*it);
        }
    }

    return result;
}

void deleteFirstOccurrence(std::vector<string> &array, string value)
{
    auto it = std::find(array.begin(), array.end(), value);

    if (it != array.end())
    {
        array.erase(it);
    }
}

void removeNonDuplicates(std::vector<std::string> &myVector)
{
    // Sort the vector
    std::sort(myVector.begin(), myVector.end());

    // Use unique technique to move unique elements to the front
    auto newEnd = std::unique(myVector.begin(), myVector.end());

    // Erase non-duplicates
    myVector.erase(newEnd, myVector.end());
}
void assignCourse(vector<Faculty> &faculties, int facultyIndex, int course_index, vector<string> course_list)
{
    Faculty &prof = faculties[facultyIndex];

    // Construct the course string
    std::string course = course_list[course_index];

    // Insert the course into the faculty's course list
    prof.faculty_courses.push_back(course);
}

void printFacultyAssignments(const vector<Faculty> &faculties)
{
    cout << "\n"
         << "Faculty Assignments:" << endl;

    for (auto prof = std::next(faculties.begin()); prof != faculties.end(); ++prof)
    {
        std::cout << "Faculty " << prof->faculty_number << " is teaching courses: ";

        if (prof->faculty_courses.empty())
        {
            std::cout << "No courses assigned." << std::endl;
        }
        else
        {
            for (const std::string &course : prof->faculty_courses)
            {
                std::cout << course << " ";
            }
            std::cout << std::endl;
        }
    }
}

vector<Faculty> readCSV(const string &filename)
{
    vector<Faculty> faculties;

    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error opening file: " << filename << endl;
        return faculties;
    }

    string line;
    while (getline(file, line))
    {
        Faculty faculty;
        stringstream ss(line);
        string value;
        string name;

        // Read name (ignored)
        getline(ss, name, ',');

        // Read category
        if (getline(ss, faculty.category, ','))
        {
            // Read faculty number as a string
            if (ss >> faculty.faculty_number)
            {
                // Read preferences
                if (getline(ss, value))
                {
                    // Remove leading and trailing whitespaces
                    value = value.substr(value.find_first_not_of(" \t") + 1);
                    value = value.substr(0, value.find_last_not_of(" \t") + 1);

                    // Split preferences string into individual strings
                    stringstream preferencesStream(value);
                    string preference;
                    while (getline(preferencesStream, preference, ','))
                    {
                        // Remove leading and trailing whitespaces and extra characters
                        preference = preference.substr(preference.find_first_not_of(" \t\"") + 1);
                        preference = preference.substr(0, preference.find_last_not_of(" \t\"") + 1);
                        preference = "C" + preference;
                        faculty.preferences.push_back(preference);
                    }
                }
            }
        }

        faculties.push_back(faculty);
    }

    file.close();
    return faculties;
}

void writeCSV(const string &inputFilename, const string &outputFilename, const vector<Faculty> &faculties)
{
    ifstream inputFile(inputFilename);
    ofstream outputFile(outputFilename);

    if (!inputFile.is_open() || !outputFile.is_open())
    {
        cerr << "Error opening files: " << inputFilename << " or " << outputFilename << endl;
        return;
    }

    // Copy the header from the input file to the output file
    string header;
    getline(inputFile, header);
    outputFile << header << ",Courses" << endl;

    // Copy the remaining lines and add the "Courses" column
    size_t index = 1;
    string line;
    while (getline(inputFile, line))
    {
        // Copy the first four columns
        outputFile << line.substr(0, line.find_last_of(' ')) << ",";

        // Add the "Courses" column based on the elements in courses vector
        if (!faculties[index].faculty_courses.empty())
        {
            // Use a loop to iterate over all courses
            outputFile << "\"";
            for (const string &course : faculties[index].faculty_courses)
            {
                outputFile << course << ",";
            }
            outputFile.seekp(-1, ios_base::end); // Remove the trailing comma
            outputFile << "\"";
        }
        else
        {
            outputFile << "\"\"";
        }

        outputFile << endl;

        index++;
    }

    inputFile.close();
    outputFile.close();
}
vector<string> reverseVector(const vector<string> &inputVector)
{
    // Create a copy of the input vector
    vector<std::string> reversedVector = inputVector;

    // Reverse the order of elements in the vector
    reverse(reversedVector.begin(), reversedVector.end());

    // Return the reversed vector
    return reversedVector;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Usage: ./go <choice>(1/2/3/4)" << endl;
        return 1; // Exit with an error code
    }
    int choice = stoi(argv[1]);
    const string inputFilename = "input.csv";
    const string outputFilename = "output.csv";

    vector<Faculty> faculties = readCSV(inputFilename);

    // We print all 12 faculty members participating in this course assignment

    for (const auto &professor : faculties)
    {
        cout << "Prof" << professor.faculty_number << ": {" << professor.category << ", "
             << professor.faculty_number << ", {";
        for (const auto &pref : professor.preferences)
        {
            cout << "\"" << pref << "\", ";
        }
        cout << "}}" << endl;
    }

    // We pushed 12 faculty members into the graph

    BipartiteGraph graph;
    // All categories are pushed into graph
    for (int i = 0; i < 13 && i < faculties.size(); ++i)
    {
        graph.faculties.push_back(faculties[i]);
    }

    vector<string> course_list = {"C1", "C2", "C4", "C3", "C5", "C6", "C7", "C8", "C9", "C10", "C11", "C12", "C13", "C14", "C15", "C16", "C1", "C2", "C4", "C3", "C5", "C6", "C7", "C8", "C9", "C10", "C11", "C12", "C13", "C14", "C15", "C16"};
    vector<string> unassigned_course_list;
    vector<string> unassigned_course_list2;

    // graph.courses = rearrangeDuplicates(course_list);
    // graph.courses = course_list;
    // graph.courses = rearrangeDuplicates(reverseVector(course_list));
    // graph.courses=reverseVector(course_list);

    if (choice == 1)
    {
        graph.courses = rearrangeDuplicates(course_list);
    }
    else if (choice == 2)
    {
        graph.courses = course_list;
    }
    else if (choice == 3)
    {
        graph.courses = rearrangeDuplicates(reverseVector(course_list));
    }
    else if (choice == 4)
    {
        graph.courses = reverseVector(course_list);
    }
    else
    {
        std::cout << "Invalid choice. Please enter 1, 2, 3, or 4." << std::endl;
        return 1; // Exit with an error code
    }

    unordered_map<int, int> facultyAssignments;
    int maxMatching = hopcroftKarp(graph, facultyAssignments);

    cout << " First assignment all three categories"
         << "\n"
         << " Maximum Bipartite Matching: " << maxMatching << endl;

    for (const auto &assignment : facultyAssignments)
    {
        int facultyIndex = assignment.first;
        int courseIndex = assignment.second;
        assignCourse(faculties, facultyIndex, courseIndex, course_list);
        course_list.erase(course_list.begin() + (courseIndex));
    }

    printFacultyAssignments(faculties);

    // Adding unassigned courses

    unassigned_course_list = course_list;

    cout << "Unassigned Courses:" << endl;
    for (const string &course : unassigned_course_list)
    {
        cout << course << " ";
    }
    cout << endl;

    // Second assignment: post-midsem courses of first assigment

    BipartiteGraph graph2;
    for (int i = 5; i < 13 && i < faculties.size(); ++i)
    {
        graph2.faculties.push_back(faculties[i]);
    }
    unassigned_course_list2 = removeDuplicateMembers(rearrangeDuplicates(unassigned_course_list));
    graph2.courses = unassigned_course_list2;

    cout << "\n"
         << "Second assignment post-midsem courses. We offer only:" << endl;
    for (const string &course : removeDuplicateMembers(rearrangeDuplicates(unassigned_course_list)))
    {
        cout << course << " ";
    }
    cout << endl;

    unordered_map<int, int> facultyAssignments2;
    int maxMatching2 = hopcroftKarp(graph2, facultyAssignments2);
    cout << "Second assignment for X2 and X3"
         << "\n"
         << "Maximum Bipartite Matching: " << maxMatching2 << endl;

    // Printing

    for (const auto &assignment2 : facultyAssignments2)
    {
        int facultyIndex2 = assignment2.first;
        int courseIndex2 = assignment2.second;
        assignCourse(faculties, facultyIndex2 + 5, courseIndex2, unassigned_course_list2);
        // Updating the unassigned course list
        deleteFirstOccurrence(unassigned_course_list, unassigned_course_list2[courseIndex2]);
    }
    printFacultyAssignments(faculties);
    cout << "Unassigned Courses list:" << endl;
    for (const string &course : unassigned_course_list)
    {
        cout << course << " ";
    }

    // Second assignment: new pre-midsem courses
    BipartiteGraph graph22;
    for (int i = 5; i < 13 && i < faculties.size(); ++i)
    {
        graph2.faculties.push_back(faculties[i]);
    }
    vector<string> unassigned_course_list22 = rearrangeDuplicates(unassigned_course_list);
    graph22.courses = unassigned_course_list22;

    cout << "\n";
    cout << "\n"
         << "Second assignment pre-midsem courses. We offer all unassigned courses now:" << endl;
    for (const string &course : unassigned_course_list)
    {
        cout << course << " ";
    }
    cout << endl;

    unordered_map<int, int> facultyAssignments22;
    int maxMatching22 = hopcroftKarp(graph22, facultyAssignments22);
    cout << "Second assignment for X2 and X3"
         << "\n"
         << "Maximum Bipartite Matching: " << maxMatching2 << endl;

    // Printing

    for (const auto &assignment22 : facultyAssignments22)
    {
        int facultyIndex22 = assignment22.first;
        int courseIndex22 = assignment22.second;
        // cout << "Faculty " << facultyIndex22 + 4 << " is teaching Course " << unassigned_course_list22[courseIndex22] << endl;
        assignCourse(faculties, facultyIndex22 + 5, courseIndex22, unassigned_course_list22);
        // Updating the unassigned course list
        deleteFirstOccurrence(unassigned_course_list, unassigned_course_list22[courseIndex22]);
    }
    printFacultyAssignments(faculties);

    cout << "Unassigned Courses list:" << endl;
    for (const string &course : unassigned_course_list)
    {
        cout << course << " ";
    }

    // Third assignment: Now using the list of unassigned courses, we assign them to X3 profs, but

    BipartiteGraph graph3;
    for (int i = 9; i < 13 && i < faculties.size(); ++i)
    {
        graph3.faculties.push_back(faculties[i]);
    }
    vector<string> unassigned_course_list3 = removeDuplicateMembers(rearrangeDuplicates(unassigned_course_list));
    graph3.courses = unassigned_course_list3;

    cout << "\n \n";
    cout << "Third Assignment post-midsem courses: We offer only:" << endl;
    for (const string &course : removeDuplicateMembers(rearrangeDuplicates(unassigned_course_list)))
    {
        cout << course << " ";
    }
    cout << endl;

    unordered_map<int, int> facultyAssignments3;
    int maxMatching3 = hopcroftKarp(graph3, facultyAssignments3);
    cout << "Third assignment for X3"
         << "\n"
         << "Maximum Bipartite Matching: " << maxMatching3 << endl;

    // Printing

    for (const auto &assignment3 : facultyAssignments3)
    {
        int facultyIndex3 = assignment3.first;
        int courseIndex3 = assignment3.second;
        // cout << "Faculty " << facultyIndex3 + 9 << " is teaching Course " << unassigned_course_list3[courseIndex3] << endl;
        assignCourse(faculties, facultyIndex3 + 10, courseIndex3, unassigned_course_list3);
        // Updating the unassigned course list
        deleteFirstOccurrence(unassigned_course_list, unassigned_course_list3[courseIndex3]);
    }

    printFacultyAssignments(faculties);
    cout << "Unassigned Courses list:" << endl;
    for (const string &course : unassigned_course_list)
    {
        cout << course << " ";
    }

    // // Third assignment: Taking care of left-over unassigned courses

    // // We will only assign full courses now for category X3 with half courses

    for (Faculty &prof : faculties)
    {
        if (prof.category == "x3" && prof.faculty_courses.size() == 1)
        {
            int coursesAdded = 0;

            for (const string &unassigned_course : unassigned_course_list)
            {
                // Check if the unassigned course is in the professor's preferences
                if (std::find(prof.preferences.begin(), prof.preferences.end(), unassigned_course) != prof.preferences.end())
                {
                    prof.faculty_courses.push_back(unassigned_course);
                    prof.faculty_courses.push_back(unassigned_course);
                    deleteFirstOccurrence(unassigned_course_list, unassigned_course);
                    deleteFirstOccurrence(unassigned_course_list, unassigned_course);
                    break;
                }
            }
        }
    }

    // // We will only assign leftover postmidsem courses now:

    for (Faculty &prof : faculties)
    {
        if (prof.category == "x3" && prof.faculty_courses.size() != 3)
        {
            int coursesAdded = 0;

            for (const string &unassigned_course : removeDuplicateMembers(unassigned_course_list))
            {
                // Check if the unassigned course is in the professor's preferences
                if (std::find(prof.preferences.begin(), prof.preferences.end(), unassigned_course) != prof.preferences.end())
                {
                    prof.faculty_courses.push_back(unassigned_course);
                    deleteFirstOccurrence(unassigned_course_list, unassigned_course);
                    break;
                }
            }
        }
    }

    printFacultyAssignments(faculties);

    // Writing the output into the file

    writeCSV(inputFilename, outputFilename, faculties);
    return 0;
}