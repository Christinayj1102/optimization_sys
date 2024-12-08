#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

// 데이터 타입 정의
typedef int data_t;

// CPU 클럭 속도 (3.5GHz = 3.5 * 10^9 Hz)
#define CPU_CLOCK_SPEED 3500000000ULL

// 벡터 구조체 정의
typedef struct {
    data_t *data; // 벡터 데이터
    int len;      // 벡터 길이
} vec, *vec_ptr;

// 벡터 생성 함수
vec_ptr new_vec(int len) {
    vec_ptr v = (vec_ptr)malloc(sizeof(vec));
    if (!v) return NULL;
    v->data = (data_t *)malloc(len * sizeof(data_t));
    if (!v->data) {
        free(v);
        return NULL;
    }
    v->len = len;
    return v;
}

// 벡터 길이 반환
int vec_length(vec_ptr v) {
    return v->len;
}

// 벡터 시작 주소 반환
data_t *get_vec_start(vec_ptr v) {
    return v->data;
}

// 벡터 초기화
vec_ptr init_vector(int size, data_t value) {
    vec_ptr v = new_vec(size);
    if (!v) return NULL;
    for (int i = 0; i < size; i++) {
        v->data[i] = value;
    }
    return v;
}

// IQR 계산을 위한 정렬 함수
int compare_doubles(const void *a, const void *b) {
    double diff = (*(double *)a) - (*(double *)b);
    return (diff > 0) - (diff < 0);
}

// IQR 기반 평균 계산
double calculate_iqr_mean(double *times, int num_trials) {
    qsort(times, num_trials, sizeof(double), compare_doubles);
    int q1_index = num_trials / 4;
    int q3_index = 3 * num_trials / 4;
    double sum = 0.0;
    int count = 0;
    for (int i = q1_index; i < q3_index; i++) {
        sum += times[i];
        count++;
    }
    return sum / count;
}

// CPE 계산 및 출력
void calculate_and_print_cpe(const char *label, double avg_time, int elements) {
    double cpe = (avg_time * CPU_CLOCK_SPEED) / elements;
    printf("%s: IQR Avg Time = %f sec, CPE = %f cycles/element\n", label, avg_time, cpe);
}

// 공통 시간 측정 함수
void measure_performance(void (*combine)(vec_ptr, data_t *, char), vec_ptr v, data_t *result, const char *label) {
    struct timespec start, end;
    int num_trials = 10000;  // 10000번 누적 실행
    double times[num_trials];

    for (int i = 0; i < num_trials; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        combine(v, result, '+');
        clock_gettime(CLOCK_MONOTONIC, &end);
        times[i] = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    }

    double iqr_avg_time = calculate_iqr_mean(times, num_trials);
    calculate_and_print_cpe(label, iqr_avg_time, vec_length(v));
}

// 덧셈 combine 함수들
void combine1(vec_ptr v, data_t *dest, char op) {
    int i;
    *dest = 0;
    for (i = 0; i < vec_length(v); i++) {
        *dest += v->data[i];
    }
}

void combine2(vec_ptr v, data_t *dest, char op) {
    int i;
    int length = vec_length(v);
    *dest = 0;
    for (i = 0; i < length; i++) {
        *dest += v->data[i];
    }
}

void combine3(vec_ptr v, data_t *dest, char op) {
    int i;
    int length = vec_length(v);
    data_t *data = get_vec_start(v);
    *dest = 0;
    for (i = 0; i < length; i++) {
        *dest += data[i];
    }
}

void combine4(vec_ptr v, data_t *dest, char op) {
    int i;
    int length = vec_length(v);
    data_t *data = get_vec_start(v);
    data_t x = 0;
    for (i = 0; i < length; i++) {
        x += data[i];
    }
    *dest = x;
}

int main() {
    int size = 1000000; // 벡터 크기
    data_t value = 1; // 벡터 초기값

    vec_ptr v = init_vector(size, value);
    if (!v) return -1;

    data_t result;

    printf("Addition results:\n");
    measure_performance(combine1, v, &result, "Combine1 (add)");
    measure_performance(combine2, v, &result, "Combine2 (add)");
    measure_performance(combine3, v, &result, "Combine3 (add)");
    measure_performance(combine4, v, &result, "Combine4 (add)");

    free(v->data);
    free(v);

    return 0;
}
