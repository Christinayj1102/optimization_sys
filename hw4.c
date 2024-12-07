#include <stdio.h>
#include <stdlib.h> // malloc, free 사용을 위해 추가
#include <time.h>   // clock() 사용을 위해 추가

#define IDENT 0   // 덧셈 초기값
#define OPER +    // 덧셈 연산자
// #define IDENT 1   // 곱셈 초기값 (사용 시 주석 해제)
// #define OPER *    // 곱셈 연산자 (사용 시 주석 해제)

typedef float data_t; // 연산 타입을 float로 설정
// typedef int data_t; // int로 바꾸려면 주석 해제

typedef struct {
    data_t *data;
    int len;
} vec, *vec_ptr;

// 벡터 생성 함수
vec_ptr new_vec(int len) {
    vec_ptr v = (vec_ptr)malloc(sizeof(vec));
    if (v == NULL) return NULL; // 메모리 할당 실패 체크
    v->data = (data_t *)malloc(len * sizeof(data_t));
    if (v->data == NULL) {
        free(v);
        return NULL; // 메모리 할당 실패 체크
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

// 벡터 요소 가져오기
int get_vec_element(vec_ptr v, int index, data_t *data) {
    if (index < 0 || index >= vec_length(v)) return 0;
    *data = v->data[index];
    return 1;
}

// combine1
void combine1(vec_ptr v, data_t *dest) {
    int i;
    *dest = IDENT;
    for (i = 0; i < vec_length(v); i++) {
        data_t val;
        get_vec_element(v, i, &val);
        *dest = *dest OPER val;
    }
}

// combine2
void combine2(vec_ptr v, data_t *dest) {
    int i;
    int length = vec_length(v);
    *dest = IDENT;
    for (i = 0; i < length; i++) {
        data_t val;
        get_vec_element(v, i, &val);
        *dest = *dest OPER val;
    }
}

// combine3
void combine3(vec_ptr v, data_t *dest) {
    int i;
    int length = vec_length(v);
    data_t *data = get_vec_start(v);
    *dest = IDENT;
    for (i = 0; i < length; i++) {
        *dest = *dest OPER data[i];
    }
}

// combine4
void combine4(vec_ptr v, data_t *dest) {
    int i;
    int length = vec_length(v);
    data_t *data = get_vec_start(v);
    data_t x = IDENT;
    for (i = 0; i < length; i++) {
        x = x OPER data[i];
    }
    *dest = x;
}

// 벡터 초기화
vec_ptr init_vector(int size, data_t value) {
    vec_ptr v = new_vec(size);
    if (v == NULL) return NULL; // 메모리 할당 실패 체크
    for (int i = 0; i < size; i++) {
        v->data[i] = value;
    }
    return v;
}

// 성능 측정
void measure_performance(void (*combine)(vec_ptr, data_t *), vec_ptr v, const char *label) {
    data_t result;
    clock_t start, end;
    start = clock();
    combine(v, &result);
    end = clock();

    // 출력 형식 지정
    if (sizeof(data_t) == sizeof(int)) {
        printf("%s: 실행 시간 = %f초, 결과 = %d\n",
               label,
               (double)(end - start) / CLOCKS_PER_SEC,
               (int)result);
    } else {
        printf("%s: 실행 시간 = %f초, 결과 = %f\n",
               label,
               (double)(end - start) / CLOCKS_PER_SEC,
               result);
    }
}

// 테스트 함수 = main 함수
int main() {
    int size = 1000000;  // 벡터 크기
    data_t value = 1.0;  // 벡터 초기값 (float로 설정)
    vec_ptr v = init_vector(size, value);
    if (v == NULL) return -1; // 메모리 할당 실패 체크

    printf("Combine 함수 성능 비교 (타입: %s, 연산: %s)\n",
           (sizeof(data_t) == sizeof(int)) ? "int" : "float",
           (IDENT == 0) ? "덧셈" : "곱셈");

    measure_performance(combine1, v, "combine1");
    measure_performance(combine2, v, "combine2");
    measure_performance(combine3, v, "combine3");
    measure_performance(combine4, v, "combine4");

    free(v->data); // 동적 메모리 해제
    free(v);       // 벡터 구조체 해제
    return 0;
}
