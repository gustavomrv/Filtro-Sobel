#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "libs/stb_image.h"
#include "libs/stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

/**
 * img_width: Largura da imagem
 * img_height: Altura da imagem
 * img_color_depth: Canais de cor da imagem
*/

int img_width, img_height, img_color_depth;
int n_files;
char* dir_in;
char* dir_out;
char* filename_in;
char* filename_out;
char** file_list;

/*******************************************************************************
 * Le uma imagem rgb.
 *
 * @param _i Indice da imagem a lida, da lista de imagens file_list. 
 * @param _data Um vetor de unsigned char que ira conter o conteudo vetorizado da imagem, no formato rgbrgbrgb... .
 * 
 * @return 1 se leu a imagem sem problemas, 0 caso contrario
*******************************************************************************/
int read_image(int i, unsigned char* &_data){
	char _f[strlen(dir_in)+strlen(file_list[i])] = ""; 
	sprintf(_f, "%s%s", dir_in, file_list[i]);
	// printf("Lendo imagem: %s\n", _f);
	_data = stbi_load(_f, &img_width, &img_height, &img_color_depth, 0);

	return (_data != NULL) ? 1 : 0;
}

/*******************************************************************************
 * Exibe no terminal o conteudo da imagem na forma vetorizada (sem formatacao)
 *
 * @param _data Um vetor de unsigned char que contem o conteudo vetorizado da imagem, no formato rgbrgbrgb...
*******************************************************************************/
void print_img_vectorized(const unsigned char* &_data){
	for (int i = 0; i < img_width*img_height*img_color_depth; i++)
		printf("%u ", (unsigned int)_data[i]);
	printf("\n");
}

/*******************************************************************************
 * Funcao que converte uma imagem de RGB para tons de cinza (Grayscale). 
 * O algoritmo utilizado considera o pesos para cada componente de acordo com .
 * 
 * @param _src Um vetor de unsigned char que ira conter a imagem RGB de origem. Os pixels sao empacotados no formato RGB.
 * Assim, como exemplo, para cada pixel p_i (p_1 p_2 p_3 ...) o formato armazenado sera (r_1 g_1 b_1 r_2 g_2 b_2 r_3 g_3 b_3...).
 * @param _dest Um vetor de unsigned char que ira conter a imagem de origem em tons de cinza (tc).
 * Assim, como exemplo, para cada pixel p_i (p_1 p_2 p_3 ...) e intensidade tc_i (que varia entre 0 e 255) o formato armazenado sera (tc_1 tc_2 tc_3 ...).
*******************************************************************************/
void rgb_to_grayscale(unsigned char* &_src, unsigned char* &_dest){
	_dest = (unsigned char*)malloc(sizeof(unsigned char)*img_width*img_height);

	unsigned int R, G, B;
	int idx = 0;

	if (img_color_depth == 3) {
		//#pragma omp parallel for num_threads(7)
		for (int i = 0; i < img_width*img_height*img_color_depth; i+=img_color_depth){				
				R = (unsigned int) _src[i];
				G = (unsigned int) _src[i+1];
				B = (unsigned int) _src[i+2];
				_dest[idx++] = (unsigned char)(0.2989*R + 0.5870*G + 0.1140*B);
		}
	} else if (img_color_depth==1){
		printf("PBPBPBPBPBB\n");
		for (int i = 0; i < img_width*img_height; i++){
			_dest[i] = _src[i];
		}
	}
}

/*******************************************************************************
 * Funcao que salva uma imagem em tons de cinza no formato vetorizado em PNG.
 * 
 * @param _i Indice da imagem a lida, da lista de imagens file_list. A esse nome sera adicionado um prefixo antes de salvar a imagem final
 * @param _data Um vetor de unsigned char que ira conter o conteudo vetorizado da imagem a ser salva, no formato rgbrgbrgb...
*******************************************************************************/
void save_image(const int i, const unsigned char* &_data){
	char prefix[] = "new";
	char _f[strlen(dir_in)+strlen(prefix)+strlen(file_list[i])] = ""; 
	sprintf(_f, "%s%s%s", dir_out, prefix, file_list[i]);

	int len = strlen(_f);
	_f[len-1] = 'g';
	_f[len-2] = 'n';
	_f[len-3] = 'p';
	// printf("Salvando imagem: %s\n", _f);

	// stbi_write_png(_f, img_width, img_height, 1, _data, 0); 
}

/*******************************************************************************
 * Funcao que salva uma matriz de inteiros, representando uma imagem em tons de cinza, em PNG.
 * 
 * @param _i Indice da imagem a lida, da lista de imagens file_list. A esse nome sera adicionado um prefixo antes de salvar a imagem final
 * @param _m Uma matriz de inteiros da imagem a ser salva, no formato tc_11 tc_12 ...
*******************************************************************************/
void save_matrix_as_image(const int i, int** &_m){
	char prefix[] = "new";
	char _f[strlen(dir_in)+strlen(prefix)+strlen(file_list[i])] = ""; 
	sprintf(_f, "%s%s%s", dir_out, prefix, file_list[i]);
	int len = strlen(_f);
	_f[len-1] = 'g';
	_f[len-2] = 'n';
	_f[len-3] = 'p';
	// printf("Salvando imagem: %s\n", _f);

	unsigned char* _data = (unsigned char*)malloc(sizeof(unsigned char)*img_width*img_height);
	int idx = 0;

	for (int linha = 0; linha < img_height; linha++){
		for (int coluna = 0; coluna < img_width; coluna++){
			_data[idx++] = _m[linha][coluna];
		}
	}	

	stbi_write_png(_f, img_width, img_height, 1, _data, 0); 
	free(_data); _data = NULL;
}

/*******************************************************************************
 * Funcao que salva uma matriz de inteiros, representando uma imagem em tons de cinza, em PGM (ASCII).
 * 
 * @param _i Indice da imagem a lida, da lista de imagens file_list. A esse nome sera adicionado um prefixo antes de salvar a imagem final
 * @param _m Uma matriz de inteiros da imagem a ser salva, no formato tc_11 tc_12 ...
*******************************************************************************/
void save_matrix_as_pgm(const int i, int** &_m){
	char prefix[] = "new";
	char _f[strlen(dir_in)+strlen(prefix)+strlen(file_list[i])] = ""; 
	sprintf(_f, "%s%s%s", dir_out, prefix, file_list[i]);
	int len = strlen(_f);
	_f[len-1] = 'm';
	_f[len-2] = 'g';
	_f[len-3] = 'p';

	FILE *f;
	f = fopen(_f, "w");
	fwrite("P2\n", sizeof(char), 3, f);
	fprintf(f, "%d %d\n%d\n", img_width, img_height, 255);
	for (int linha = 0; linha < img_height; linha++){
		for (int coluna = 0; coluna < img_width; coluna++){
			fprintf(f, "%d ",_m[linha][coluna]);
		}
		fprintf(f, "%s", "\n");
	}	
	fclose(f);
}

/*******************************************************************************
 * Funcao que converte uma imagem representada na forma vetorizada em uma representacao por matriz de inteiros, em tons de cinza.
 * 
 * @param _src Um vetor de unsigned char que contem a imagem de origem em tons de cinza (tc) no formato (tc_1 tc_2 tc_3 ...).
 * @param _m Uma matriz de inteiros da imagem a ser salva, no formato tc_11 tc_12 ...
*******************************************************************************/
void convert_to_matrix(unsigned char* &_src, int** &_m){
	// Aloca matriz
	_m = (int**) malloc (img_height*(sizeof(int*)));
	//#pragma omp parallel for num_threads(7)
	for (int i = 0; i<img_height; i++)
		_m[i] = (int*) malloc (img_width * sizeof(int));

	// Converte da representacao vetorizada para matricial
	//#pragma omp parallel for num_threads(7)
	for (int linha = 0; linha < img_height; linha++){
		for (int coluna = 0; coluna < img_width; coluna++){
			_m[linha][coluna] = _src[linha*img_width + coluna];
		}
	}
}

/*******************************************************************************
 * Exibe no terminal o conteudo da lista de imagens a serem processadas
 *
 * @param _filename Nome do arquivo que contem a lista de arquivos a serem processados.
*******************************************************************************/
void read_filelist(const char* _filename){
	FILE* f;
	int line_length = 100;
	char line[line_length];
	n_files = 0;

	f = fopen(_filename, "r");
	if (f){
		// Conta total de arquivos
		while(fgets(line, line_length, f) != NULL){
			n_files++;
		}	
		rewind(f);
		// Aloca vetor de n_files strings
		file_list = (char**) malloc(n_files*sizeof(char*));
		int id_file = 0;
		long int new_line_size;
		while(fgets(line, line_length, f) != NULL){
			// Remove quebra de linha		
			new_line_size = strcspn(line, "\n");						
			line[new_line_size] = '\0';
			// Adiciona nome do arquivo no vetor de arquivos a serem processados
			file_list[id_file] = (char*) malloc ((new_line_size+1)*sizeof(char));
			sprintf(file_list[id_file], "%s", line);
			file_list[id_file][new_line_size] = '\0';
			id_file++;
		}
	}
	fclose(f);
}

/*******************************************************************************
 * Exibe no terminal o conteudo da lista de imagens a serem processadas
*******************************************************************************/
void print_filelist(){
	printf("Total de arquivos que serao lidos: %d\n", n_files);
	for (int i = 0; i<n_files; i++)
		printf("Arquivo: %s\n", file_list[i]);
}

int main(int argc, char** argv){
	
	if (argc > 3){    
		dir_in = argv[1];
		dir_out = argv[2];

		read_filelist(argv[3]);
		print_filelist();

		printf("Iniciando processamento...\n");
		unsigned char** img_rgb = NULL;
		unsigned char** img_gray = NULL;
		int** img_matrix = NULL;
		int** matriz_final = NULL;
		int i;

		int sobel_horizontal[3][3] = {
			{1, 0, -1},
			{2, 0, -2},
			{1, 0, -1}
		};

		int sobel_vertical[3][3] = {
			{1, 2, 1},
			{0, 0, 0},
			{-1, -2, -1}
		};

		double start_time = omp_get_wtime();

		// Itera sobre lista de arquivos
		img_rgb = (unsigned char**) malloc (sizeof(unsigned char*));
		img_gray = (unsigned char**) malloc (sizeof(unsigned char*));

		//#pragma omp single
		#pragma omp parallel for private(img_matrix, matriz_final)
		for (i = 0; i < n_files; i++){				
			if (read_image(i, *img_rgb)){ // Le imagem
				rgb_to_grayscale(*img_rgb, *img_gray);        // Converte para tons de cinza
				matriz_final = NULL;
				img_matrix = NULL;
				convert_to_matrix(*img_gray, img_matrix);     // Transforma em uma matriz
				convert_to_matrix(*img_gray, matriz_final);
				// Faz o processamento que precisar na matrix img_matrix 
				// 1080 x 1920

				//#pragma omp parallel num_threads(1)
				
				//#pragma omp parallel for
				#pragma omp critical 
				{
				for (int j = 1; j < img_height - 1; j++) {
					for (int k = 1; k < img_width - 1; k++) {
						int sum = 0;

						// Aplica o filtro Sobel 
						for (int x = -1; x <= 1; x++) {
							for (int y = -1; y <= 1; y++) {
								sum += img_matrix[j + x][k + y] * sobel_horizontal[x + 1][y + 1];
							}
						}
						
						// Calcula o gradiente combinando horizontal e vertical
						int gradient = sqrt(sum * sum);

						// Limita os valores resultantes entre 0 e 255
						gradient = gradient > 255 ? 255 : gradient < 0 ? 0 : gradient;

						// Atribui o resultado ao ponto correspondente na nova matriz ou na mesma matriz
						matriz_final[j][k] = gradient;
					}
				}
				


				// Salva imagem final (Apenas para os temas de convolucao)		

				save_matrix_as_image(i, matriz_final);
				}
				
				// save_matrix_as_pgm(i, img_matrix);

				// Libera memoria das imgs
				// printf("Liberando memória da imagem %s\n", file_list[i]);
				stbi_image_free(*img_rgb); *img_rgb = NULL;
				stbi_image_free(*img_gray); *img_gray = NULL;
				for (int l = 0; l < img_height; l++) {
					free(img_matrix[l]); img_matrix[l] = NULL;
					free(matriz_final[l]); matriz_final[l] = NULL;
				}
				free(img_matrix);
				free(matriz_final);
					
			} else {
				printf("Erro lendo imagem %s\n", file_list[i]);
			}
		}
		
		double end_time = omp_get_wtime();
		double calc_time = end_time - start_time;

		printf("Processamento encerrado após %lf segundos!\n\nLiberando memoria e finalizando aplicacao.\n", calc_time);

		// Libera memoria da lista de arquivos
		for (int i = 0; i<n_files; i++)
			free(file_list[i]);
		free(file_list);
		free(img_rgb);
		free(img_gray);
		
	} else {
		printf("Voce deve chamar a aplicacao passando, respectivamente: \n");
		printf("./nomeaplicacao diretorio_entrada diretorio_saida lista_arquivos_a_processar.\n");
	}

	return 0;
}
