#include <fstream>

#include <opencv2/imgcodecs.hpp>
#include <dirent.h>
#include "smoke.hpp"
#include "trt_modulated_deform_conv.hpp"

#define BATCH_SIZE 1

static inline int read_files_in_dir(const char *p_dir_name, std::vector<std::string> &file_names) {
    DIR *p_dir = opendir(p_dir_name);
    if (p_dir == nullptr) {
        return -1;
    }

    struct dirent *p_file = nullptr;
    while ((p_file = readdir(p_dir)) != nullptr) {// readdir 读取目录中的下一个文件名
        if (strcmp(p_file->d_name, ".") != 0 &&
            strcmp(p_file->d_name, "..") != 0) {
            //std::string cur_file_name(p_dir_name);
            //cur_file_name += "/";
            //cur_file_name += p_file->d_name;
            std::string cur_file_name(p_file->d_name);
            file_names.push_back(cur_file_name);// push_back()函数将一个新的元素加到vector的最后面
        }
    }

    closedir(p_dir);
    return 0;
}

int main(int argc, char **argv) {
    auto path = argv[1];

    cv::Mat kitti_img = cv::imread(path);
    cv::Mat intrinsic = (cv::Mat_<float>(3, 3) <<
                                               721.5377, 0.0, 609.5593, 0.0, 721.5377, 172.854, 0.0, 0.0, 1.0);
    // SMOKE smoke("../smoke_dla34.engine", intrinsic);
    // smoke.Detect(kitti_img);
    SMOKE smoke;
    smoke.prepare(intrinsic);
    std::string onnx_path = "../smoke_dla34.onnx";
    std::string engine_path = "../smoke_dla34_fp32.engine";
    std::ifstream f(engine_path.c_str());
    bool engine_file_exist = f.good();

    if (engine_file_exist) {
        smoke.LoadEngine(engine_path);
//        for(int i = 0;i < 2;i++)
//        {
//            smoke.Detect(kitti_img);
//        }

    } else {
        smoke.LoadOnnx(onnx_path);
    }

    std::string img_dir = "/media/ros/A666B94D66B91F4D/ros/learning/deploy/tensorrt_smoke/data2";
    std::vector<std::string> file_names;
    if (read_files_in_dir(img_dir.c_str(), file_names) < 0) {
        std::cerr << "read_files_in_dir failed." << std::endl;
        return -1;
    }

    std::sort(file_names.begin(), file_names.end());

    int fcount = 0;
    for (int f = 0; f < (int) file_names.size(); f++) {
        fcount++;
        if (fcount < BATCH_SIZE && f + 1 != (int) file_names.size()) continue;
        for (int b = 0; b < fcount; b++) {
            cv::Mat img = cv::imread(img_dir + "/" + file_names[f - fcount + 1 + b]);
            smoke.Detect(img, file_names[f - fcount + 1 + b]);  // "0000000017.png"
        }
        fcount = 0;
    }

    return 0;
}
