#include <stdio.h>
#include <stdlib.h> //for exit
int main(void) {
	char fname[] = "grade.txt";
	char name[80];
	FILE *f;
	int cnt = 0;
	if ((f = fopen(fname, "w")) == NULL) { //���� ���� ���� ��
		puts("���Ͽ��� ����"); //����Ϳ� ǥ��
		exit(1);
	}
	printf("�̸��� �Է��ϼ��� \n");
	fgets(name, 80, stdin);
	while (!feof(stdin)) {//�Է¹��� ������ ���� �ƴ϶�� ���Ͽ� ����
		fprintf(f, "%d", ++cnt);
		fputs(name, f);
		puts("again \n");
		fgets(name, 80, stdin); //�ٽ� �Է� ����
	}
	fclose(f);


	/*
	char text[] = "55512345";
	int i;
	FILE *fp = fopen("num.txt", "w");
	for (i = 0; i < sizeof(text) - 1; i++)
		fputc(text[i], fp); //���Ͽ� �迭���� ����
	fclose(fp);
	*/
}