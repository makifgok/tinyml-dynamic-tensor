#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Eski C standartlari icin ozel yuvarlama makrosu */
#define ROUND_VAL(x) ((int)((x) < 0 ? (x) - 0.5 : (x) + 0.5))

/* Desteklenen veri tipleri */
typedef enum {
    TENSOR_FP32,
    TENSOR_FP16, 
    TENSOR_INT8
} DataType;

/* Hafizadan tasarruf saglayan Union yapisi */
typedef union {
    float* fp32_data;
    uint16_t* fp16_data;
    int8_t* int8_data;
    void* raw; 
} TensorData;

/* Dinamik Tensor Struct'i */
typedef struct {
    int rows;
    int cols;
    DataType type;
    TensorData data;
    float scale;
    int8_t zero_point;
} Tensor;

/* Tensor olusturma fonksiyonu */
Tensor* create_tensor(int rows, int cols, DataType type) {
    Tensor* t = (Tensor*)malloc(sizeof(Tensor));
    int size = rows * cols;
    
    t->rows = rows;
    t->cols = cols;
    t->type = type;

    if (type == TENSOR_FP32) {
        t->data.fp32_data = (float*)malloc(size * sizeof(float));
    } else if (type == TENSOR_FP16) {
        t->data.fp16_data = (uint16_t*)malloc(size * sizeof(uint16_t));
    } else if (type == TENSOR_INT8) {
        t->data.int8_data = (int8_t*)malloc(size * sizeof(int8_t));
    }
    return t;
}

/* Belleði serbest birakma */
void free_tensor(Tensor* t) {
    free(t->data.raw);
    free(t);
}

/* FP32 bir tensoru INT8'e Quantize etme */
Tensor* quantize_fp32_to_int8(Tensor* fp32_tensor) {
    Tensor* int8_tensor;
    int size, i, quantized_val;
    float min_val, max_val, real_val;

    size = fp32_tensor->rows * fp32_tensor->cols;
    int8_tensor = create_tensor(fp32_tensor->rows, fp32_tensor->cols, TENSOR_INT8);

    min_val = fp32_tensor->data.fp32_data[0];
    max_val = fp32_tensor->data.fp32_data[0];
    
    /* Min ve Max degerleri bulma */
    for (i = 1; i < size; i++) {
        if (fp32_tensor->data.fp32_data[i] < min_val) min_val = fp32_tensor->data.fp32_data[i];
        if (fp32_tensor->data.fp32_data[i] > max_val) max_val = fp32_tensor->data.fp32_data[i];
    }

    /* Scale ve Zero Point hesaplama */
    int8_tensor->scale = (max_val - min_val) / 255.0f;
    int8_tensor->zero_point = -128 - ROUND_VAL(min_val / int8_tensor->scale);

    /* Verileri donusturme ve yeni tensore yazma */
    for (i = 0; i < size; i++) {
        real_val = fp32_tensor->data.fp32_data[i];
        quantized_val = ROUND_VAL(real_val / int8_tensor->scale) + int8_tensor->zero_point;
        
        /* Sinirlandirma (Clipping) */
        if (quantized_val > 127) quantized_val = 127;
        if (quantized_val < -128) quantized_val = -128;
        
        int8_tensor->data.int8_data[i] = (int8_t)quantized_val;
    }

    return int8_tensor;
}

int main() {
    Tensor* t_float;
    Tensor* t_quantized;
    int i;

    printf("--- TinyML Dinamik Tensor Bellek Yonetimi ---\n");

    /* 1. FP32 Tensor Olustur */
    t_float = create_tensor(2, 2, TENSOR_FP32);
    t_float->data.fp32_data[0] = 0.5f;
    t_float->data.fp32_data[1] = 1.2f;
    t_float->data.fp32_data[2] = -0.8f;
    t_float->data.fp32_data[3] = 3.1f;

    printf("\n[FP32 Tensor Orijinal Degerler (32-bit)]\n");
    for(i = 0; i < 4; i++) {
        printf("%f ", t_float->data.fp32_data[i]);
    }

    /* 2. Quantization Islemi */
    t_quantized = quantize_fp32_to_int8(t_float);

    printf("\n\n[INT8 Tensor Quantize Degerler (8-bit - Bellek Tasarrufu!)]\n");
    for(i = 0; i < 4; i++) {
        printf("%d ", t_quantized->data.int8_data[i]);
    }

    /* 3. Bellek Tuketimi Karsilastirmasi */
    printf("\n\n[Bellek Analizi]\n");
    printf("FP32 Matris Bellek Boyutu: %lu byte\n", 4 * sizeof(float));
    printf("INT8 Matris Bellek Boyutu: %lu byte\n", 4 * sizeof(int8_t));
    printf("Tasarruf Orani: %%75\n");

    free_tensor(t_float);
    free_tensor(t_quantized);
    
    return 0;
}
