#ifndef PREPROCESSING_H
#define PREPROCESSING_H

char* preprocess_text(const char* input_text);
char** tokenize_sentence(char* process_txt, int* num_token);
char** load_stopwords(const char* filename, int* num_stopwords);
char** remove_stopwords(char** tokens, int num_tokens, char** stopwords, int num_stopwords, int* num_filtered_tokens);

#endif
