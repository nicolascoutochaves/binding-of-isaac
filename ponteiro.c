/*Fa�a uma fun��o que recebe dois valores inteiros, n1 e n2. A fun��o tamb�m deve utilizar 3
ponteiros atrav�s dos quais s�o retornados: a adi��o (n1+n2), a subtra��o (n1-n2) e a
multiplica��o (n1*n2) dos valores informados.*/
#include <stdio.h>

void contas(float n1, float n2, float * ptr1,float * ptr2,float * ptr3)
{
    *ptr1= n1 + n2;
    *ptr2= n1 - n2;
    *ptr3= n1 * n2;
}
int main()
{
    float ptr1,ptr2,ptr3,n1,n2,op;

    printf("insira n1 e n2:\n");
    scanf("%f%f",&n1,&n2);

    contas(n1,n2, &ptr1, &ptr2, &ptr3);
    printf("o valor da adicao eh: %.2f\n da subtracao eh: %.2f\n da multiplicacao eh: %.2f\n",ptr1,ptr2,ptr3);

}
