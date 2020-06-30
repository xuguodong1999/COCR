#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char **argv) {
    Mat img = imread("/home/xgd/Pictures/backup/4.jpg");
    if (img.empty()) {
        cout << "打开图像失败！" << endl;
        return -1;
    }
    imshow("image", img);
    waitKey();

    return 0;
}


//
//#include <iostream>
//#include <vector>
//
//using namespace std;
//
//void heapify(int arr[], int n, int i) {
//    int largest = i; // 将最大元素设置为堆顶元素
//    int l = 2 * i + 1; // left = 2*i + 1
//    int r = 2 * i + 2; // right = 2*i + 2
//
//    // 如果 left 比 root 大的话
//    if (l < n && arr[l] > arr[largest])
//        largest = l;
//
//    // I如果 right 比 root 大的话
//    if (r < n && arr[r] > arr[largest])
//        largest = r;
//
//    if (largest != i) {
//        swap(arr[i], arr[largest]);
//
//        // 递归地定义子堆
//        heapify(arr, n, largest);
//    }
//}
//
//void heapSort(int arr[], int n) {
//    // 建立堆
//    for (int i = n /2- 1; i >= 0; i--)
//        heapify(arr, n, i);
//    for (int i = 0; i < n; i++) {
//        cout << arr[i] << ", ";
//    }
//    cout << endl;
//    // 一个个从堆顶取出元素
//    for (int i = n - 1; i >= 0; i--) {
//        swap(arr[0], arr[i]);
//        heapify(arr, i, 0);
//    }
//}
//
//int main() {
//    vector<int> arr = {1, 7, 2, 6, 3, 4};
//    heapSort(arr.data(), arr.size());
//    for (auto &i:arr) {
//        cout << i << ", ";
//    }
//    cout << endl;
//    return 0;
//}