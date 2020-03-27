//
// Created by Administrator on 2020/3/16.
//
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

#define Path "C:/apk/simplest_mediadata_test-master/simplest_mediadata_test-master/simplest_mediadata_test/lena_256x256_yuv420p.yuv"
#define Y "C:/apk/output_420_y.y"
#define U "C:/apk/output_420_u.y"
#define V "C:/apk/output_420_v.y"

int simplest_yuv420_split(char *url, int w, int h, int num) {
    FILE *fp = fopen(url, "rb+");
    FILE *fp1 = fopen(Y, "wb+");
    FILE *fp2 = fopen(U, "wb+");
    FILE *fp3 = fopen(V, "wb+");

    unsigned char *pic = (unsigned char *) malloc(w * h * 3 / 2);

    for (int i = 0; i < num; i++) {

        fread(pic, 1, w * h * 3 / 2, fp);
        //Y
        fwrite(pic, 1, w * h, fp1);
        //U
        fwrite(pic + w * h, 1, w * h / 4, fp2);
        //V
        fwrite(pic + w * h * 5 / 4, 1, w * h / 4, fp3);
    }

    free(pic);
    fclose(fp);
    fclose(fp1);
    fclose(fp2);
    fclose(fp3);

    return 0;
}


void test() {

}

void init_string() {
    vector<char> vc = {'h', 'e', 'l'};
    string s(&vc[0], 8);
    cout << s << endl;
}

void assign_string() {
    string s("hello world!");
    cout << s[6] << endl;
    s.insert(6, "c++");
    cout << s << endl;
    s.erase(20, 20);
    cout << "affter:\t";
    cout << s << endl;
}

void test_std_string_method() {
    int i = 100;
    string s = to_string(i);
    cout << "to_string()\t" << s << endl;
    double d = stod(s);
    cout << "stod()\t" << d << endl;
}

void test_algorithm() {
    vector<int> vi = {1, 2, 3, 4, 5};
    cout << "vi-acc:\t" << accumulate(vi.cbegin(), vi.cend(), 0) << endl;
    vector<string> vs = {"Hello", "C++", ",I'm coming"};
    cout << "vs-acc\t" << accumulate(vs.cbegin(), vs.cend(), string("")) << endl;
    vector<int> v;
    fill_n(v.begin(), v.size(), 0);
    cout << "v-fill_n(size()):\t";
    for (int &ref:v) {
        cout << ref << '\t';
    }
    cout << endl;

    vector<int> v2;
    fill_n(back_inserter(v2), 10, 10);
    cout << "v2_fill_n_10:\t";
    for (int &ref:v2) {
        cout << ref << '\t';
    }
    cout << endl;

    vector<int> v3;
    auto it = back_inserter(v3);
    *it = 520;
    cout << "v3:\t";
    for (int &ref:v3) {
        cout << ref << '\t';
    }
    cout << endl;

    int a1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int a2[sizeof(a1) / sizeof(*a1)];
    auto ret = copy(begin(a1), end(a1), a2);
    cout << "copy:";
    for (int &a:a2) {
        cout << a << '\t';
    }
    cout << endl;

}

void pdf() {
    fstream fs("", ios_base::in);

}

void split_yuv() {
    fstream fstrm(Path, ios_base::in);
    fstrm.seekg(0, fstrm.end);
    long size = fstrm.tellg();
    fstrm.seekg(0);

    long ysize = size * 2 / 3;
    long usize = size / 6;
    char *y = new char[ysize];
    char *u = new char[usize];
    char *v = new char[usize];

    fstrm.read(y, ysize);
    fstrm.read(u, usize);
    fstrm.read(v, size - (ysize + usize));

    fstream out_fstrm;
    out_fstrm.open(Y, ios_base::out);
    out_fstrm.write(y, ysize);
    out_fstrm.close();

    out_fstrm.open(U, ios_base::out);
    out_fstrm.write(u, usize);
    out_fstrm.close();

    out_fstrm.open(V, ios_base::out);
    out_fstrm.write(v, usize);
    out_fstrm.close();
}

int main() {

    //split_yuv()
    //simplest_yuv420_split(Path,256,256,1);
    //test();
    //init_string();
    //assign_string();
    //test_std_string_method();
    test_algorithm();
    return 0;
}
