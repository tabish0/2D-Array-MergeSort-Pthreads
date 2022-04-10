#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>

int** arr;
int* tempSum;

typedef struct Pair {
	int low;
	int high;
	int rowId;
}Pair;

void merge(int low, int mid, int high, int rowId)
{
	int i, j, k;
	int n1 = mid - low + 1;
	int n2 = high - mid;

	int* left = (int*)malloc((mid - low + 1) * sizeof(int));
	int* right = (int*)malloc((high - mid) * sizeof(int));

	for (i = 0; i < n1; i++)
		left[i] = arr[rowId][low + i];
	for (j = 0; j < n2; j++)
		right[j] = arr[rowId][mid + 1 + j];

	i = 0;
	j = 0;
	k = low;
	while (i < n1 && j < n2)
	{
		if (left[i] >= right[j])
		{
			arr[rowId][k] = left[i];
			i++;
		}
		else
		{
			arr[rowId][k] = right[j];
			j++;
		}
		k++;
	}
	while (i < n1)
	{
		arr[rowId][k] = left[i];
		i++;
		k++;
	}

	while (j < n2)
	{
		arr[rowId][k] = right[j];
		j++;
		k++;
	}

	free(left);
	free(right);
}

void* mergeSort(void* arg)
{
	Pair pair = *((Pair*)arg);
	int low = pair.low;
	int high = pair.high;
	int rowId = pair.rowId;

	if (low < high)
	{
		int mid = low + (high - low) / 2;

		pthread_t thread1;
		Pair pair1 = { low, mid,rowId };

		pthread_t thread2;
		Pair pair2 = { mid + 1, high,rowId };

		pthread_create(&thread1, NULL, mergeSort, &pair1);
		pthread_create(&thread2, NULL, mergeSort, &pair2);

		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);

		merge(low, mid, high, rowId);
	}
}

void* wrapperMergeSort(void* arg)
{
	Pair pair = *((Pair*)arg);
	int* row = arr[pair.rowId];
	tempSum[pair.rowId] = 0;
	for (int i = 0; i <= pair.high; ++i)
	{
		tempSum[pair.rowId] += arr[pair.rowId][i];
	}
	mergeSort(&pair);
}

int main()
{
	int m, n, sum;
	printf("Enter the number of rows \n");
	scanf_s("%d", &m);
	printf("Enter the number of cols \n");
	scanf_s("%d", &n);

	pthread_t* threads = (pthread_t*)malloc(m * sizeof(pthread_t));

	tempSum = (int*)malloc(m * sizeof(int));

	arr = (int**)malloc(m * sizeof(int*));
	for (int i = 0; i < m; i++)
		arr[i] = (int*)malloc(n * sizeof(int));


	printf("Starting array\n");
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			arr[i][j] = rand() % 100;
			printf("%d ", arr[i][j]);
		}
		printf("\n");
	}
	printf("\n");


	for (int i = 0; i < m; ++i)
	{
		Pair pair = { 0, n - 1, i };
		pthread_create(&threads[i], NULL, wrapperMergeSort, &pair);
		pthread_join(threads[i], NULL);
	}

	sum = 0;

	for (int i = 0; i < m; ++i)
	{
		sum += tempSum[i];
	}

	printf("Sorted array is: \n");
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			printf("%d ", arr[i][j]);
		}
		printf("\n");
	}

	printf("Matrix Addition = %d", sum);

	free(threads);
	free(tempSum);
	free(arr);

	return 0;
}