/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";


static int gen_rand_num() {
  return rand() % 100; 
}

static char gen_rand_op() {
  char ops[] = {'+', '-', '*', '/'};
  return ops[rand() % 4];
}

static void gen_rand_expr() {
  buf[0] = '\0';

  int num_expr = rand() % 5 + 1; // 生成 1 到 5 个表达式

  for (int i = 0; i < num_expr; i++) {
      if (i > 0) {
          strcat(buf, " ");
      }

      int num1 = gen_rand_num();
      char op = gen_rand_op();
      int num2 = gen_rand_num();

      // 避免除数为 0
      while (op == '/' && num2 == 0) {
          num2 = gen_rand_num();
      }

      char temp[100];
      sprintf(temp, "%d %c %d", num1, op, num2);
      strcat(buf, temp);

      if (i < num_expr - 1) {
          strcat(buf, " + "); // 用 + 连接多个表达式
      }
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
