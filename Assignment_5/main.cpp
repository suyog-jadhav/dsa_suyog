#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

struct ImageNode {
    string path;
    Mat image;
    ImageNode* next;
    ImageNode* prev;
    
    ImageNode(string& imagePath) : path(imagePath), next(NULL), prev(NULL) {}
};

class ImageViewer {
private:
    ImageNode* head;
    ImageNode* current;
    string filepath;
    
    // Supported image formats
    vector<string> supportedFormats;
    
    void initializeSupportedFormats() {
        supportedFormats.push_back(".jpg");
        supportedFormats.push_back(".jpeg");
        supportedFormats.push_back(".png");
        supportedFormats.push_back(".bmp");
        supportedFormats.push_back(".tiff");
        supportedFormats.push_back(".tif");
        supportedFormats.push_back(".webp");
        supportedFormats.push_back(".jp2");
    }
    
    string toLowerCase(string str) {
        for (int i = 0; i < str.length(); i++) {
            if (str[i] >= 'A' && str[i] <= 'Z') {
                str[i] = str[i] + 32;
            }
        }
        return str;
    }
    
    string getFileExtension(string& path) {
        size_t dotPos = path.find_last_of(".");
        if (dotPos != string::npos && dotPos < path.length() - 1) {
            return toLowerCase(path.substr(dotPos));
        }
        return "";
    }
    
    bool isSupportedFormat(string& path) {
        string ext = getFileExtension(path);
        for (int i = 0; i < supportedFormats.size(); i++) {
            if (supportedFormats[i] == ext) {
                return true;
            }
        }
        return false;
    }
    
    string trim(string& str) {
        if (str.empty()) return str;
        
        // Find first non-space character
        size_t first = 0;
        while (first < str.length() && str[first] == ' ') {
            first++;
        }
        
        if (first == str.length()) return ""; // All spaces
        
        // Find last non-space character
        size_t last = str.length() - 1;
        while (last > first && str[last] == ' ') {
            last--;
        }
        
        return str.substr(first, last - first + 1);
    }
    
    bool fileExists(string& path) {
        ifstream file(path.c_str());
        bool exists = file.good();
        file.close();
        return exists;
    }

public:
    ImageViewer() : head(NULL), current(NULL), filepath("image_path.txt") {
        initializeSupportedFormats();
    }
    
    ~ImageViewer() {
        clear();
    }
    
    void setFile(string& file) {
        if (file.empty()) {
            cerr << "Error: Empty file path provided!" << endl;
            return;
        }
        filepath = file;
    }
    
    void preload() {
        ifstream f1;
        f1.open(filepath.c_str());
        if (!f1.is_open()) {
            cerr << "Warning: Could not open file '" << filepath << "'. File may not exist." << endl;
            return;
        }
        
        string temp;
        int lineNumber = 0;
        int loadedCount = 0;
        
        while (getline(f1, temp)) {
            lineNumber++;
            temp = trim(temp);
            
            // Skip empty lines and comments
            if (temp.empty() || temp[0] == '#') {
                continue;
            }
            
            if (insertImage(temp)) {
                loadedCount++;
            }
        }
        
        f1.close();
        cout << "Loaded " << loadedCount << " images from " << filepath << endl;
    }
    
    bool storedata() {
        if (!head) {
            cout << "No images to save." << endl;
            return true;
        }
        
        ofstream f2;
        f2.open(filepath.c_str());
        if (!f2.is_open()) {
            cerr << "Error: Could not open file '" << filepath << "' for writing!" << endl;
            return false;
        }
        
        f2 << "# Image paths - auto-generated" << endl;
        
        ImageNode* temp = head;
        int count = 0;
        do {
            f2 << temp->path << endl;
            temp = temp->next;
            count++;
            
            // Safety check to prevent infinite loop
            if (count > 10000) {
                cerr << "Warning: Stopped saving after 10000 images (possible circular reference)" << endl;
                break;
            }
        } while (temp != head);
        
        f2.close();
        cout << "Saved " << count << " image paths to " << filepath << endl;
        return true;
    }
    
    bool insertImage(string& path) {
        if (path.empty()) {
            cerr << "Error: Empty image path provided!" << endl;
            return false;
        }
        
        string cleanPath = trim(path);
        if (cleanPath.empty()) {
            cerr << "Error: Path contains only whitespace!" << endl;
            return false;
        }
        
        // Check if path exists
        if (!fileExists(cleanPath)) {
            cerr << "Error: File does not exist or is not accessible: " << cleanPath << endl;
            return false;
        }
        
        // Check if format is supported
        if (!isSupportedFormat(cleanPath)) {
            cerr << "Error: Unsupported image format: " << cleanPath << endl;
            return false;
        }
        
        // Check if image is already loaded
        if (isImageLoaded(cleanPath)) {
            cout << "Image already loaded: " << cleanPath << endl;
            return false;
        }
        
        Mat img;
        img = imread(cleanPath.c_str(), IMREAD_COLOR);
        
        if (img.empty()) {
            cerr << "Error: Cannot load or decode image: " << cleanPath << endl;
            return false;
        }
        
        // Check image dimensions
        if (img.rows == 0 || img.cols == 0) {
            cerr << "Error: Invalid image dimensions: " << cleanPath << endl;
            return false;
        }
        
        ImageNode* newNode = new ImageNode(cleanPath);
        newNode->image = img.clone(); // Deep copy to ensure data integrity
        
        if (!head) {
            head = newNode;
            head->next = head;
            head->prev = head;
            current = head;
        } else {
            ImageNode* last = head->prev;
            last->next = newNode;
            newNode->prev = last;
            newNode->next = head;
            head->prev = newNode;
            current = newNode;
        }
        
        cout << "Successfully inserted: " << cleanPath << " (" << img.cols << "x" << img.rows << ")" << endl;
        return true;
    }
    
    bool isImageLoaded(string& path) {
        if (!head) return false;
        
        ImageNode* temp = head;
        int count = 0;
        do {
            if (temp->path == path) {
                return true;
            }
            temp = temp->next;
            count++;
            // Safety check
            if (count > 10000) break;
        } while (temp != head);
        
        return false;
    }
    
    bool deleteCurrent() {
        if (!current) {
            cout << "No image to delete!" << endl;
            return false;
        }
        
        ImageNode* delNode = current;
        string deletedPath = delNode->path;
        
        if (current->next == current) {
            // Only one node
            head = NULL;
            current = NULL;
        } else {
            delNode->prev->next = delNode->next;
            delNode->next->prev = delNode->prev;
            if (delNode == head) {
                head = delNode->next;
            }
            current = delNode->next;
        }
        
        delete delNode;
        cout << "Deleted: " << deletedPath << endl;
        return true;
    }
    
    void nextImage() {
        if (current && current->next) {
            current = current->next;
        }
    }
    
    void prevImage() {
        if (current && current->prev) {
            current = current->prev;
        }
    }
    
    bool show() {
        if (!current) {
            cout << "No images loaded!" << endl;
            return false;
        }
        
        if (current->image.empty()) {
            cerr << "Error: Current image data is corrupted!" << endl;
            return false;
        }
        
        string windowName = "Image Viewer";
        namedWindow(windowName.c_str(), WINDOW_NORMAL | WINDOW_KEEPRATIO);
        
        // Set larger window size constraints for bigger images
        int maxW = 1920, maxH = 1080;
        int minW = 800, minH = 600;
        
        // Calculate scale while maintaining aspect ratio - allow upscaling for small images
        double scaleW = (double)maxW / current->image.cols;
        double scaleH = (double)maxH / current->image.rows;
        double scale = (scaleW < scaleH) ? scaleW : scaleH;
        
        // Allow upscaling up to 2x for small images, but limit huge images
        if (scale > 2.0) scale = 2.0; // Max 2x upscale
        if (scale < 0.1) scale = 0.1; // Min scale to prevent tiny images
        
        int newW = (int)(current->image.cols * scale);
        int newH = (int)(current->image.rows * scale);
        if (newW < minW) newW = minW;
        if (newH < minH) newH = minH;
        
        resizeWindow(windowName.c_str(), newW, newH);
        
        Mat resized;
        if (scale != 1.0) {
            // Use different interpolation based on upscaling or downscaling
            int interpolation = (scale > 1.0) ? INTER_CUBIC : INTER_AREA;
            resize(current->image, resized, Size(newW, newH), 0, 0, interpolation);
        } else {
            resized = current->image;
        }
        
        imshow(windowName.c_str(), resized);
        
        // Extract filename from path manually
        string filename = current->path;
        size_t lastSlash = filename.find_last_of("/\\");
        if (lastSlash != string::npos && lastSlash < filename.length() - 1) {
            filename = filename.substr(lastSlash + 1);
        }
        
        cout << "Showing: " << current->path << " (" << filename << ")" << endl;
        return true;
    }
    
    bool isEmpty() {
        return head == NULL;
    }
    
    void clear() {
        if (!head) return;
        
        ImageNode* temp = head;
        int count = 0;
        do {
            ImageNode* next = temp->next;
            delete temp;
            temp = next;
            count++;
            
            // Safety check to prevent infinite loop
            if (count > 10000) {
                cerr << "Warning: Stopped clearing after 10000 nodes (possible corruption)" << endl;
                break;
            }
        } while (temp != head && temp != NULL);
        
        head = NULL;
        current = NULL;
        cout << "Cleared " << count << " images from memory." << endl;
    }
    
    int getImageCount() {
        if (!head) return 0;
        
        ImageNode* temp = head;
        int count = 0;
        do {
            count++;
            temp = temp->next;
            if (count > 10000) break; // Safety check
        } while (temp != head);
        
        return count;
    }
    
    void printStatus() {
        int count = getImageCount();
        cout << "Status: " << count << " images loaded";
        if (current) {
            // Extract filename manually
            string filename = current->path;
            size_t lastSlash = filename.find_last_of("/\\");
            if (lastSlash != string::npos && lastSlash < filename.length() - 1) {
                filename = filename.substr(lastSlash + 1);
            }
            cout << ", currently viewing: " << filename;
        }
        cout << endl;
    }
};

// Safe input function to handle potential input errors
string getInput(string prompt) {
    cout << prompt;
    string input;
    if (!getline(cin, input)) {
        // Handle EOF or input error
        if (cin.eof()) {
            cout << "\nEnd of input detected. Exiting..." << endl;
            exit(0);
        }
        cin.clear();
        cin.ignore(10000, '\n');
        return "";
    }
    return input;
}

int main() {
    ImageViewer viewer;
    
    cout << "=== Enhanced Image Viewer ===" << endl;
    cout << "Supported formats: .jpg, .jpeg, .png, .bmp, .tiff, .tif, .webp, .jp2" << endl;
    
    // Try to preload images
    viewer.preload();
    viewer.printStatus();
    
    cout << "\nControls:" << endl;
    cout << "← (Arrow Left)  : Previous image" << endl;
    cout << "→ (Arrow Right) : Next image" << endl;
    cout << "i : Insert new image" << endl;
    cout << "d : Delete current image" << endl;
    cout << "o : Open new path.txt file" << endl;
    cout << "s : Save current list" << endl;
    cout << "c : Clear all images" << endl;
    cout << "h : Show help" << endl;
    cout << "q : Quit" << endl;
    
    bool running = true;
    while (running) {
        if (!viewer.isEmpty()) {
            if (!viewer.show()) {
                cerr << "Failed to display image. Continuing..." << endl;
            }
            
            int key = waitKey(0);
            
            switch (key) {
                case 'q':
                case 27: // ESC key
                    cout << "Exiting..." << endl;
                    running = false;
                    break;
                    
                case 81:  // Left arrow (Linux)
                case 2424832: // Left arrow (Windows)
                    viewer.prevImage();
                    break;
                    
                case 83:  // Right arrow (Linux)
                case 2555904: // Right arrow (Windows)
                    viewer.nextImage();
                    break;
                    
                case 'd':
                    viewer.deleteCurrent();
                    if (viewer.isEmpty()) {
                        destroyAllWindows();
                        cout << "All images deleted." << endl;
                    }
                    break;
                    
                case 'i': {
                    destroyAllWindows(); // Close window temporarily for input
                    string path = getInput("Enter image path: ");
                    if (!path.empty()) {
                        viewer.insertImage(path);
                    }
                    break;
                }
                
                case 'o': {
                    destroyAllWindows();
                    viewer.clear();
                    string newFile = getInput("Enter new path.txt file: ");
                    if (!newFile.empty()) {
                        viewer.setFile(newFile);
                        viewer.preload();
                        viewer.printStatus();
                    }
                    break;
                }
                
                case 's':
                    destroyAllWindows();
                    if (viewer.storedata()) {
                        cout << "Image list saved successfully." << endl;
                    }
                    break;
                    
                case 'c':
                    destroyAllWindows();
                    viewer.clear();
                    break;
                    
                case 'h':
                    destroyAllWindows();
                    cout << "\n=== Help ===" << endl;
                    cout << "Navigation: Use arrow keys" << endl;
                    cout << "Management: 'i' to insert, 'd' to delete, 'c' to clear all" << endl;
                    cout << "File ops: 'o' to open new list, 's' to save current list" << endl;
                    cout << "Other: 'h' for help, 'q' or ESC to quit" << endl;
                    cout << "Current status:" << endl;
                    viewer.printStatus();
                    cout << "Press any key to continue..." << endl;
                    cin.get();
                    break;
                    
                default:
                    // Ignore unknown keys
                    break;
            }
        } else {
            // No images loaded
            cout << "\nNo images loaded! Choose option:" << endl;
            cout << " i : Insert new image" << endl;
            cout << " o : Open a path.txt file" << endl;
            cout << " q : Quit" << endl;
            cout << "Choice: ";
            
            char choice;
            if (!(cin >> choice)) {
                if (cin.eof()) {
                    cout << "End of input. Exiting..." << endl;
                    break;
                }
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
            }
            cin.ignore(); // Clear newline
            
            switch (choice) {
                case 'q':
                    running = false;
                    break;
                    
                case 'i': {
                    string path = getInput("Enter image path: ");
                    if (!path.empty()) {
                        viewer.insertImage(path);
                    }
                    break;
                }
                
                case 'o': {
                    string newFile = getInput("Enter new path.txt file: ");
                    if (!newFile.empty()) {
                        viewer.setFile(newFile);
                        viewer.preload();
                        viewer.printStatus();
                    }
                    break;
                }
                
                default:
                    cout << "Invalid choice. Please try again." << endl;
                    break;
            }
        }
    }
    
    // Cleanup
    if (!viewer.storedata()) {
        cerr << "Warning: Failed to save image list before exit." << endl;
    }
    
    destroyAllWindows();
    cout << "Goodbye!" << endl;
    return 0;
}
