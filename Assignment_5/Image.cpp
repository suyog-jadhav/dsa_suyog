#include <opencv2/opencv.hpp>
#include <iostream>
#include<fstream>
#include <string>
using namespace cv;
using namespace std;

struct ImageNode {
    string path;
    Mat image;
    ImageNode* next;
    ImageNode* prev;
    ImageNode(string p, Mat img) {
        path = p;
        image = img;
        next = prev = nullptr;
    }
};

class ImageViewer {
private:
    ImageNode* head;
    ImageNode* current;
public:
    ImageViewer() {
        head = nullptr;
        current = nullptr;
    }
    
    void preload(){
        ifstream f1;
        f1.open("image_path.txt",ios::in);
        if(!f1){
            cout<<"File is empty!!\n";
            return;
        }
        string temp;
        while(!f1.eof()){
            getline (f1,temp);
            if(!temp.empty()) {  
                insertImage(temp);
            }
        }
        f1.close();
        return;
    }
    
    void storedata(){
        ofstream f2;
        f2.open("image_path.txt",ios::out);
        if(!f2){
            cout<<"File not found!!\n";
            return;
        }
        if(head) {  
            ImageNode *temp=head;
            do {
                f2 << temp->path << "\n";
                temp = temp->next;
            } while (temp != head);
        }
        f2.close();
        return;
    }    
        
    void insertImage(string path) {
        Mat img = imread(path);
        if (img.empty()) {
            cout << " Cannot load: " << path << endl;
            return;
        }
        ImageNode* newNode = new ImageNode(path, img);
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
        cout << "Inserted: " << path << endl;
    }
    
    void deleteCurrent() {
        if (!current) {
            cout << "No image to delete!" << endl;
            return;
        }
        ImageNode* delNode = current;
        if (current->next == current) {
            head = nullptr;
            current = nullptr;
        } else {
            delNode->prev->next = delNode->next;
            delNode->next->prev = delNode->prev;
            if (delNode == head) head = delNode->next;
            current = delNode->next;
        }
        cout << "Deleted: " << delNode->path << endl;
        delete delNode;
    }
    
    void nextImage() {
        if (current) 
            current = current->next;
    }
    
    void prevImage() {
        if (current) 
            current = current->prev;
    }
    
    void show() {
        if (!current) {
            cout << "No images loaded!" << endl;
            return;
        }
        namedWindow("Image Viewer", WINDOW_NORMAL);
        resizeWindow("Image Viewer", 1280, 720);
        // keep aspect ratio
        Mat resized;
        int maxW = 1280, maxH = 720;
        double scale = min((double)maxW / current->image.cols,
                           (double)maxH / current->image.rows);
        resize(current->image, resized, Size(), scale, scale);
        imshow("Image Viewer", resized);
        cout << "Showing: " << current->path << endl;
    }
    
    bool isEmpty() {
        return head == nullptr;
    }
};

int main() {
    ImageViewer viewer;
    viewer.preload();
    cout << "\nControls:\n";
    cout << "-> : Next image\n";
    cout << "<- : Previous image\n";
    cout << "i : Insert new image\n";
    cout << "d : Delete current image\n";
    cout << "q : Quit\n";
    
    while (true) {
        viewer.show();
        int key = waitKey(0); // wait for key press
        
        // Print key code for debugging
        cout << "Key pressed: " << key << endl;
        
        if (key == 'q' || key == 27) {  // quit (q or ESC)
            cout << "Exiting...\n";
            break;
        }
        // Try multiple possible key codes for right arrow
        else if (key == 83) {  // Right arrow
            viewer.nextImage();
            cout << "Next image\n";
        }
        // Try multiple possible key codes for left arrow  
        else if (key == 81) {  // Left arrow
            viewer.prevImage();
            cout << "Previous image\n";
        }
        else if (key == 'd') {
            viewer.deleteCurrent();
            if (viewer.isEmpty()) {
                cout << "No images left. Exiting.\n";
                break;
            }
        }
        else if (key == 'i') {
            destroyWindow("Image Viewer");  // close old window so it doesn't overlap
            string path;
            cout << "Enter image path: ";
            cin >> path;
            viewer.insertImage(path);
        }
        // Alternative: use WASD or number keys
        else if (key == 'a') {  // 'a' for previous
            viewer.prevImage();
            cout << "Previous image\n";
        }
        else if (key == 'd' && key != 'd') {  // This condition will never be true, removed
        }
        else if (key == 's') {  // 's' for next  
            viewer.nextImage();
            cout << "Next image\n";
        }
    }
    viewer.storedata();
    destroyAllWindows();
    return 0;
}
