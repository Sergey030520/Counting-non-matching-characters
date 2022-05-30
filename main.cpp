#include <iostream>
#include "omp.h"
#include "mpi.h"
#include <time.h>

#include <numeric>

#include <stdlib.h>

using namespace std;

int countNumberMismatchedCharacters(string word1, string word2){
    int countRepeat = abs((int)(word2.size() - word1.size()));
    for (int ind = 0; ind < (word1.size() < word2.size() ? word1.size() : word2.size()); ++ind) {
        if(word1.at(ind) != word2.at(ind)) ++countRepeat;
    }
    return countRepeat;
}

int countNumberMismatchedCharactersParallel(string word1, string word2){
    int countRepeat = abs((int) (word2.size() - word1.size()));

#pragma omp parallel for reduction(+:countRepeat)
    for (int ind = 0; ind < (word1.size() < word2.size() ? word1.size() : word2.size()); ++ind) {
        if (word1.at(ind) != word2.at(ind)) {
            ++countRepeat;
        }
    }
    return countRepeat;
}
string readFileText(MPI_File *file, const int rank, const int size, const int overlap) {
    MPI_Offset startIt, endIt, filesize;
    int size_file;
    char *text;


    MPI_File_get_size(*file, &filesize);

    size_file = filesize / size;
    startIt = rank * size_file;
    endIt = startIt + size_file;
    if (rank == size - 1) endIt = filesize - 1;
    else if(rank != size - 1) endIt += overlap;

    size_file = endIt - startIt + 1;

    text = new char[(size_file + 1) * sizeof(char)];
    MPI_File_read_at_all(*file, startIt, text, size_file, MPI_CHAR, MPI_STATUS_IGNORE);
    text[size_file] = '\0';
    return text;
}

MPI_File* openFile(const string& path){
    MPI_File *file = new MPI_File;
    int error;
#pragma omp critical
    {
        error = MPI_File_open(MPI_COMM_WORLD, path.c_str(), MPI_MODE_RDONLY, MPI_INFO_NULL, file);
    }
    if(error != MPI_SUCCESS){
        fprintf(stderr, "%s: MPI_File_open() returned in error\n", path.c_str());
        return nullptr;
    }
    return file;
}

void show_work_time(clock_t start, clock_t end){
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    printf("The time: %f seconds\n", seconds);
}

template<typename t>
void show(string name_value, t res){
    cout << "Result " << name_value << ": " << res << endl;
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
/*Получает ранг вызывающего процесса в группе указанного коммуникатора.*//*

    MPI_Comm_size(MPI_COMM_WORLD, &size);*/
/*Получает общее количество доступных процессов.*/


    MPI_File *file1(nullptr), *file2(nullptr);
    string text1, text2;

#pragma omp parallel
    {
#pragma omp sections
        {
#pragma omp section
            {
                file1 = openFile(R"(C:\Users\Malip\CLionProjects\Counting non-matching characters\text1.txt)");
            }
#pragma omp section
            {
                file2 = openFile(R"(C:\Users\Malip\CLionProjects\Counting non-matching characters\text2.txt)");
            }
        }
#pragma omp sections
        {
#pragma omp section
            {
                text1 = readFileText(file1, rank, size, 100);
            }
#pragma omp section
            {
                text2 = readFileText(file2, rank, size, 100);
            }
        }
    }

    MPI_File_close(file1);
    MPI_File_close(file2);
    delete file1;
    delete file2;

    int count;
    clock_t time_start = clock();
    count = countNumberMismatchedCharacters(text1, text2);
    clock_t time_end = clock();

    show("count number mismatched characters", count);
    show_work_time(time_start, time_end);

    time_start = clock();
    count = countNumberMismatchedCharactersParallel(text1, text2);
    time_end = clock();

    show("count number mismatched characters (parallel)", count);
    show_work_time(time_start, time_end);

    MPI_Finalize();
}