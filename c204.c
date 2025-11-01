/* ******************************* c204.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c204 - Převod infixového výrazu na postfixový (s využitím c202)     */
/*  Referenční implementace: Petr Přikryl, listopad 1994                      */
/*  Přepis do jazyka C: Lukáš Maršík, prosinec 2012                           */
/*  Upravil: Kamil Jeřábek, září 2019                                         */
/*           Daniel Dolejška, září 2021                                       */
/* ************************************************************************** */
/*
** Implementujte proceduru pro převod infixového zápisu matematického výrazu
** do postfixového tvaru. Pro převod využijte zásobník (Stack), který byl
** implementován v rámci příkladu c202. Bez správného vyřešení příkladu c202
** se o řešení tohoto příkladu nepokoušejte.
**
** Implementujte následující funkci:
**
**    infix2postfix ... konverzní funkce pro převod infixového výrazu
**                      na postfixový
**
** Pro lepší přehlednost kódu implementujte následující pomocné funkce:
**
**    untilLeftPar ... vyprázdnění zásobníku až po levou závorku
**    doOperation .... zpracování operátoru konvertovaného výrazu
**
** Své řešení účelně komentujte.
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c204.h"

// bool solved;
// 8 bits
#define BYTE 8

// Func returns true if char is opening parentheses
bool Is_LeftPar(char topChar) {
  return (topChar == '(') || (topChar == '{') || (topChar == '[');
}
// Func returns true if char is closing parentheses
bool Is_RightPar(char topChar) {
  return (topChar == ')') || (topChar == '}') || (topChar == ']');
}
// Func return priority num based on operator
int precedence(char operator) {
  switch (operator) {
  case '+':
  case '-':
    return 1;

  case '*':
  case '/':
    return 2;

  case '^':
    return 3;
  }
  return -1;
}
// Func returns if symbol is valid operand
bool IsOperand(char symbol) {
  return ((symbol >= '0') && (symbol <= '9')) ||
         ((symbol >= 'A') && (symbol <= 'Z')) ||
         ((symbol >= 'a') && (symbol <= 'z'));
}
// Func that returns value defined for specific variable
bool ret_VarValue(char var, VariableValue variableValues[],
                  int variableValueCount, int *varValue) {
  for (int var_idx = 0; var_idx < variableValueCount; var_idx++) {
    //printf("Comparing %c with %c\n", var, variableValues[var_idx].name);
    if (variableValues[var_idx].name == var) {
      *varValue = variableValues[var_idx].value;
      return true;
    }
  }
  return false;
}
// Func that performs specific operation based on parsed operator
int performOperation(int operand1, int operand2, char operator) {
  switch (operator) {
  case '+':
    return operand1 + operand2;
  case '-':
    return operand1 - operand2;
  case '*':
    return operand1 * operand2;
  case '/':
    // Division by zero is handled by caller (eval). Assume non-zero here.
    return operand1 / operand2;
  default:
    return 0;
  }
}

/**
 * Pomocná funkce untilLeftPar.
 * Slouží k vyprázdnění zásobníku až po levou závorku, přičemž levá závorka bude
 * také odstraněna.
 * Pokud je zásobník prázdný, provádění funkce se ukončí.
 *
 * Operátory odstraňované ze zásobníku postupně vkládejte do výstupního pole
 * znaků postfixExpression.
 * Délka převedeného výrazu a též ukazatel na první volné místo, na které se má
 * zapisovat, představuje parametr postfixExpressionLength.
 *
 * Aby se minimalizoval počet přístupů ke struktuře zásobníku, můžete zde
 * nadeklarovat a používat pomocnou proměnnou typu char.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param postfixExpression Znakový řetězec obsahující výsledný postfixový výraz
 * @param postfixExpressionLength Ukazatel na aktuální délku výsledného
 * postfixového výrazu
 */
void untilLeftPar(Stack *stack, char *postfixExpression,
                  unsigned *postfixExpressionLength) {
  char topChar;
  if (!Stack_IsEmpty(stack)) {
    Stack_Top(stack, &topChar);
     //printf("ulp: %c\n", topChar);
    // Clears whole stack until closing parentheses
    // also appends operator to result expression
    while (!Stack_IsEmpty(stack) && !Is_LeftPar(topChar)) {
      postfixExpression[*postfixExpressionLength] = topChar;
       //printf("postExpr: %s\n", postfixExpression);
      (*postfixExpressionLength)++;
      Stack_Pop(stack);
      if (!Stack_IsEmpty(stack)) {
        Stack_Top(stack, &topChar);
      }
    }
    // Remove the left par if present
    if (!Stack_IsEmpty(stack) && Is_LeftPar(topChar)) {
      Stack_Pop(stack);
    }
  }
}

/**
 * Pomocná funkce doOperation.
 * Zpracuje operátor, který je předán parametrem c po načtení znaku ze
 * vstupního pole znaků.
 *
 * Dle priority předaného operátoru a případně priority operátoru na vrcholu
 * zásobníku rozhodneme o dalším postupu.
 * Délka převedeného výrazu a taktéž ukazatel na první volné místo, do kterého
 * se má zapisovat, představuje parametr postfixExpressionLength, výstupním
 * polem znaků je opět postfixExpression.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param c Znak operátoru ve výrazu
 * @param postfixExpression Znakový řetězec obsahující výsledný postfixový výraz
 * @param postfixExpressionLength Ukazatel na aktuální délku výsledného
 * postfixového výrazu
 */
void doOperation(Stack *stack, char c, char *postfixExpression,
                 unsigned *postfixExpressionLength) {
  char topChar;
  while (!Stack_IsEmpty(stack)) {
    Stack_Top(stack, &topChar);
     //printf("dO-c: %c\n", topChar);
     if (Is_LeftPar(topChar)) break;
    // Checks if parsed operator has higher/lower priority than operator on
    // stack
    if (precedence(topChar) >= precedence(c)) {
      postfixExpression[*postfixExpressionLength] = topChar;
      (*postfixExpressionLength)++;
      Stack_Pop(stack);
    } else {
      break;
    }
  }
  Stack_Push(stack, c);
}

/**
 * Konverzní funkce infix2postfix.
 * Čte infixový výraz ze vstupního řetězce infixExpression a generuje
 * odpovídající postfixový výraz do výstupního řetězce (postup převodu hledejte
 * v přednáškách nebo ve studijní opoře).
 * Paměť pro výstupní řetězec (o velikosti MAX_LEN) je třeba alokovat. Volající
 * funkce, tedy příjemce konvertovaného řetězce, zajistí korektní uvolnění zde
 * alokované paměti.
 *
 * Tvar výrazu:
 * 1. Výraz obsahuje operátory + - * / ve významu sčítání, odčítání,
 *    násobení a dělení. Sčítání má stejnou prioritu jako odčítání,
 *    násobení má stejnou prioritu jako dělení. Priorita násobení je
 *    větší než priorita sčítání. Všechny operátory jsou binární
 *    (neuvažujte unární mínus).
 *
 * 2. Hodnoty ve výrazu jsou reprezentovány jednoznakovými identifikátory
 *    a číslicemi - 0..9, a..z, A..Z (velikost písmen se rozlišuje).
 *
 * 3. Ve výrazu může být použit předem neurčený počet dvojic kulatých
 *    závorek. Uvažujte, že vstupní výraz je zapsán správně (neošetřujte
 *    chybné zadání výrazu).
 *
 * 4. Každý korektně zapsaný výraz (infixový i postfixový) musí být uzavřen
 *    ukončovacím znakem '='.
 *
 * 5. Při stejné prioritě operátorů se výraz vyhodnocuje zleva doprava.
 *
 * Poznámky k implementaci
 * -----------------------
 * Jako zásobník použijte zásobník znaků Stack implementovaný v příkladu c202.
 * Pro práci se zásobníkem pak používejte výhradně operace z jeho rozhraní.
 *
 * Při implementaci využijte pomocné funkce untilLeftPar a doOperation.
 *
 * Řetězcem (infixového a postfixového výrazu) je zde myšleno pole znaků typu
 * char, jenž je korektně ukončeno nulovým znakem dle zvyklostí jazyka C.
 *
 * Na vstupu očekávejte pouze korektně zapsané a ukončené výrazy. Jejich délka
 * nepřesáhne MAX_LEN-1 (MAX_LEN i s nulovým znakem) a tedy i výsledný výraz
 * by se měl vejít do alokovaného pole. Po alokaci dynamické paměti si vždycky
 * ověřte, že se alokace skutečně zdrařila. V případě chyby alokace vraťte
 * namísto řetězce konstantu NULL.
 *
 * @param infixExpression vstupní znakový řetězec obsahující infixový výraz k
 * převedení
 *
 * @returns znakový řetězec obsahující výsledný postfixový výraz
 */
char *infix2postfix(const char *infixExpression) {
  Stack stack;
  char *postfixExpression = malloc(MAX_LEN * sizeof(char));

  // Check if malloc was successful
  if (postfixExpression == NULL) {
    return NULL;
  }

  // Null the string
  postfixExpression[0] = '\0';
  unsigned postfixExpressionLength = 0;
  Stack_Init(&stack);
  // Loops through infix expression and convert it into postfix based on
  // specific conditions
  for (unsigned str_idx = 0; infixExpression[str_idx] != '\0'; str_idx++) {
    char currentChar = infixExpression[str_idx];
     //printf("Parsed char: %c\n", currentChar);
    // When parsed char is operand, append it to final string
    if (IsOperand(currentChar)) {
       //printf("Operand: %c\n", currentChar);
      postfixExpression[postfixExpressionLength] = currentChar;
      postfixExpressionLength++;
    } else if (Is_LeftPar(currentChar)) {
       //printf("Left Par: %c\n", currentChar);
      Stack_Push(&stack, currentChar);
    } else if (Is_RightPar(currentChar)) {
       //printf("Right Par: %c\n", currentChar);
      untilLeftPar(&stack, postfixExpression, &postfixExpressionLength);
      // The end of our expression
    } else if (currentChar == '=') {
      // Clears the stack and appends = as ending symbol to posfix expression
      while (!Stack_IsEmpty(&stack)) {
        char topChar;
        Stack_Top(&stack, &topChar);
        postfixExpression[postfixExpressionLength++] = topChar;
        Stack_Pop(&stack);
      }
      postfixExpression[postfixExpressionLength++] = '=';
      break;
    } else {
       //printf("Operator: %c\n", currentChar);
      // When parsed char is operator
      doOperation(&stack, currentChar, postfixExpression,
                  &postfixExpressionLength);
    }
  }

  // End the string with null terminator and clean up
  postfixExpression[postfixExpressionLength] = '\0';
  Stack_Dispose(&stack);
  return postfixExpression;
}

/**
 * Pomocná metoda pro vložení celočíselné hodnoty na zásobník.
 *
 * Použitá implementace zásobníku aktuálně umožňuje vkládání pouze
 * hodnot o velikosti jednoho byte (char). Využijte této metody
 * k rozdělení a postupné vložení celočíselné (čtyřbytové) hodnoty
 * na vrchol poskytnutého zásobníku.
 *
 * @param stack ukazatel na inicializovanou strukturu zásobníku
 * @param value hodnota k vložení na zásobník
 */
void expr_value_push(Stack *stack, int value) {
  for (int byte_idx = sizeof(int) - 1; byte_idx >= 0; byte_idx--) {
    // Push each byte of int to stack (big-endian, MSB->LSB)
    char byte = (value >> (BYTE * byte_idx)) & 0xFF;
    Stack_Push(stack, byte);
  }
}

/**
 * Pomocná metoda pro extrakci celočíselné hodnoty ze zásobníku.
 *
 * Využijte této metody k opětovnému načtení a složení celočíselné
 * hodnoty z aktuálního vrcholu poskytnutého zásobníku. Implementujte
 * tedy algoritmus opačný k algoritmu použitému v metodě
 * `expr_value_push`.
 *
 * @param stack ukazatel na inicializovanou strukturu zásobníku
 * @param value ukazatel na celočíselnou proměnnou pro uložení
 *   výsledné celočíselné hodnoty z vrcholu zásobníku
 */
void expr_value_pop(Stack *stack, int *value) {
  *value = 0;
  // Pop byte from top of the stack and store it into int from LSB to MSB
  for (unsigned long byte_idx = 0; byte_idx < sizeof(int); byte_idx++) {
    char byte;
    Stack_Top(stack, &byte);
    Stack_Pop(stack);
    *value |= ((unsigned char)byte) << (BYTE * byte_idx);
  }
}

/**
 * Tato metoda provede vyhodnocení výrazu zadaném v `infixExpression`,
 * kde hodnoty proměnných použitých v daném výrazu jsou definovány
 * v poli `variableValues`.
 *
 * K vyhodnocení vstupního výrazu využijte implementaci zásobníku
 * ze cvičení c202. Dále také využijte pomocných funkcí `expr_value_push`,
 * respektive `expr_value_pop`. Při řešení si můžete definovat libovolné
 * množství vlastních pomocných funkcí.
 *
 * Předpokládejte, že hodnoty budou vždy definovány
 * pro všechy proměnné použité ve vstupním výrazu.
 *
 * @param infixExpression vstupní infixový výraz s proměnnými
 * @param variableValues hodnoty proměnných ze vstupního výrazu
 * @param variableValueCount počet hodnot (unikátních proměnných
 *   ve vstupním výrazu)
 * @param value ukazatel na celočíselnou proměnnou pro uložení
 *   výsledné hodnoty vyhodnocení vstupního výrazu
 *
 * @returns true v případě úspěšného vyhodnocení daného výrazu
 * 	 na základě poskytnutých hodnot proměnných, false jinak
 */
bool eval(const char *infixExpression, VariableValue variableValues[],
          int variableValueCount, int *value) {
  // Init stack for eval
  Stack stack;
  Stack_Init(&stack);

  // Convert infix to postfix
  char *postfixExpr;
  postfixExpr = infix2postfix(infixExpression);
  // printf("postfix: %s\n", postfixExpr);
  //  Check if conversion failed
  if (postfixExpr == NULL) {
    Stack_Dispose(&stack);
    // printf("conversion failed\n");
    return false;
  }

  // printf("postfix: %s\n", postfixExpr);
  //  Cycle through our postfixExpr until end symbol (=)
  for (int expr_idx = 0; postfixExpr[expr_idx] != '='; expr_idx++) {
    char currentChar = postfixExpr[expr_idx];
    // printf("Current char: %c\n", currentChar);
    //  When current char is operand push it to eval stack
    if (IsOperand(currentChar)) {
      int varValue;
      if (currentChar >= '0' && currentChar <= '9') {
        varValue = currentChar - '0'; // Convert to int
      } else {
        if (!ret_VarValue(currentChar, variableValues, variableValueCount,
                          &varValue)) {
          // Variable not found
          //printf("Variable not found\n");
          Stack_Dispose(&stack);
          free(postfixExpr);
          return false;
        }
      }
      expr_value_push(&stack, varValue);
      // When current char is operator (binary) pop two operands and perform
      // operation
    } else {
      // Check if there is enough operands
      if (Stack_IsEmpty(&stack)) {
        Stack_Dispose(&stack);
        free(postfixExpr);
        return false;
      }

      int operand2;
      expr_value_pop(&stack, &operand2);
      //printf("operand2: %d\n", operand2);

      // Check if there is enough operands
      if (Stack_IsEmpty(&stack)) {
        Stack_Dispose(&stack);
        free(postfixExpr);
        return false;
      }

      int operand1;
      expr_value_pop(&stack, &operand1);
      //printf("operand1: %d\n", operand1);

      // Guard against division by zero -> evaluation fails
      if (currentChar == '/' && operand2 == 0) {
        Stack_Dispose(&stack);
        free(postfixExpr);
        return false;
      }

      *value = performOperation(operand1, operand2, currentChar);
      //printf("result: %d\n", *value);
      expr_value_push(&stack, *value);
    }
  }
  // Last pop of final result
  expr_value_pop(&stack, value);

  // Clean up
  Stack_Dispose(&stack);
  free(postfixExpr);
  return true;
}

/*
int main () {
  char *postfixExpr;
  char *infixExpr = "(A*0+b/l)-((c+D)*(x)/k*8)*(e/(3*f/g+r/t*(j+o/p)))=";
  postfixExpr = infix2postfix(infixExpr);
  printf("Result: %s\n", postfixExpr);
  //Stack s;
  //Stack_Init(&s);
  //int value = -1;
  //expr_value_push(&s, value);
  //expr_value_pop(&s, &value);
  //printf("value: %d\n", value);
  free(postfixExpr);
  //We could say that ("0*undefined var=") returns 0
  //but we expect all vars to be defined
  char *infixExpression = "(A*0+b/l)-((c+D)*(x)/k*5)*(e/(3*f/g+h/i*(j+k/m)))=";
  int res = 0;
  VariableValue vars3[] = {
      {'A',99},{'b',20},{'c',51},{'D',149},{'e',26350},{'f',500},{'g',20},{'h',350},{'i',10},{'j',15},{'k',20},{'l',5},{'m',4},{'x',-10}
   };
  bool test = eval(infixExpression, vars3, 14, &res);
  printf("res: %d\nbool: %d\n", res, test);
  return 0;
}
*/
/* Konec c204.c */
