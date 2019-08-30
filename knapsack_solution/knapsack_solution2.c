//19MM319
//齊藤健斗

//
//こちらは同じアイテムを複数選択しないパターン
//内容物の表示がうまくいかなかったのでDPテーブルを表示しました
//
//コンパイル
//gcc -o knapsack_solution2 knapsack_solution2.c
//
//実行
//knapsack_solution2 knapsack.txt
//引数としてテキストファイルを入れてください
//


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <windows.h>


#define MAX_ITEMS 256
#define IMAX 100
#define JMAX 100

int dp[IMAX+1][JMAX+1];

struct Item{
int Item_weight;
int Item_value;
};

int main(int argc, char *argv[]){
	FILE *inputfp;
	int capacity = 0;
	int num_of_Item = 0;
	struct Item *Items ;
	int i = 0,temp, j;
	int buf[MAX_ITEMS];
	
	
	
	//	input data from file(argv[1])-------------------------------
	if (argc == 2) {
	
		inputfp = fopen(argv[1], "r");
		if(inputfp == NULL) {
			fprintf(stderr, "%sを開くことができません", argv[1]);
			exit(2);
		}
		
		//最大容積を格納
		fscanf(inputfp, "%d", &capacity);
		
		//アイテムの個数を知る		
		while(fscanf(inputfp,"%d %d",&buf, &buf) != EOF){
		num_of_Item++;
		}
		
		fseek(inputfp,0L,SEEK_SET);
		
		//メモリ領域確保
		Items = (struct Item *)malloc(sizeof(struct Item) * num_of_Item);
		fscanf(inputfp, "%d", &capacity);
		
		//配列に格納
		while(fscanf(inputfp,"%d %d",&(Items[i].Item_weight), &(Items[i].Item_value)) != EOF){
		i++;
		}

		fclose(inputfp);
	} else {
		fprintf(stderr, "引数のエラーです\n");
		exit(1);
	}
	
	printf("\n-----input information-----\n");
	printf("capacity : %d\n", capacity);
	for(i = 0; i < num_of_Item; i++){
	printf("Items[%d] : %d, %d\n",i, Items[i].Item_weight, Items[i].Item_value);
	}
	printf("number of items :  %d\n", num_of_Item);
	//input end ------------------------------------
	
	
	//dp初期化
	
	for(int w = 0; w <= capacity; ++w) dp[0][w] = 0;

	//DP
	for(int i = 0; i < num_of_Item; ++i){
		for(int w = 0; w <= capacity; ++w){	
			if(w >= Items[i].Item_weight){
				dp[i+1][w] = max(dp[i][w-Items[i].Item_weight] + Items[i].Item_value, dp[i][w]);
			}else{
				dp[i+1][w] = dp[i][w];
			}
		}
	}
	
	printf("\n-----result-----");
		printf("\nmax value (dp[n][W]): %d\n",dp[num_of_Item][capacity]);


		printf("\nDP table (i行 w列)\n");
		for(int i = 0; i < num_of_Item+1; i++){
			for(int j = 0; j < capacity+1; j++){
				printf(" %2d",dp[i][j]);
			}
			printf("\n");
		}
	
	return 0;
}


