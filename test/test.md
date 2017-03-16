# Test program

This program converts small letters to capital letters. 

If you enter some string, then this program will check whether each character is small letter or not. If small letter, the letter will be changed into capital letter. 

> Input : user's input (string)
> Output : user's input which is converted to the capital letter 

Pseudocode is like following. 

``` C
void main(void){
	int i; 
	char *buf = malloc( /*some amount */ );

	printf( "user input : " );
	gets( buf );
	
	for ( i=0 ; i< /* some amout */ ; i++ ){
		if ( 0x61 <= buf[i] && buf[i] <= 0x7a ) {
			buf[i] -= 20; 
		}
		else if (buf[i] == 0) {
			break; 
		}
	}

	exit(0);
}
```