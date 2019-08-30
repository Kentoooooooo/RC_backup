//19MM319
//齊藤健斗
//
//こちらは同じアイテムを複数選択できるパターン
//内容物を表示できるようにしました
//
//コンパイル
//gcc -o knapsack_solution knapsack_solution.c
//
//実行
//knapsack_solution knapsack.txt
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
	
	printf("\n-----input information-----\n\n");
	printf("capacity : %d\n", capacity);
	for(i = 0; i < num_of_Item; i++){
	printf("Items[%d] : %d, %d\n",i, Items[i].Item_weight, Items[i].Item_value);
	}
	printf("number of items :  %d\n", num_of_Item);
	//input end ------------------------------------
	
	
	//dp初期化
	int choices[capacity + 1];
	int maxValues[capacity + 1];

	for(i = 0; i < capacity + 1; i++){
	maxValues[i] = 0;
	choices[i] = -1;
	}
	
	int l = 0;
	
	//DPループ
	for(i = 0; i < num_of_Item; i++){
		for(j = Items[i].Item_weight; j < capacity + 1; j++){
			temp = maxValues[j - Items[i].Item_weight] + Items[i].Item_value;
			if(temp > maxValues[i]){
				maxValues[j] = temp;
				choices[j] = i;
			}
		}
	}


	printf("\n-----result-----\n");
	printf("\nmax value = %d\n",maxValues[capacity]);	

	printf("\nSelected items...\n");
	i = capacity;
	while (choices[i] >= 0){
		j = choices[i];
		printf("Items[%2d]: %2d, %2d\n",j,Items[j].Item_weight, Items[j].Item_value);
		i -= Items[choices[i]].Item_weight;
	}
	
	
	return 0;
}


