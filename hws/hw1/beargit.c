#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - You are given the following helper functions:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the homework spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_init(void) {
  fs_mkdir(".beargit");

  FILE* findex = fopen(".beargit/.index", "w");
  fclose(findex);
  
  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");

  return 0;
}


/* beargit add <filename>
 * 
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR: File <filename> already added
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char* filename) {
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      fprintf(stderr, "ERROR: File %s already added\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 3;
    }

    fprintf(fnewindex, "%s\n", line);
  }

  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");

  return 0;
}


/* beargit rm <filename>
 * 
 * See "Step 2" in the homework 1 spec.
 *
 */

int beargit_rm(const char* filename) {
  /* COMPLETE THE REST */
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");

  int flag = 0;
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
	  flag = 1;
	  continue;
      return 3;
    }
    fprintf(fnewindex, "%s\n", line);
  }
  if(!flag){
      fprintf(stderr, "ERROR: File %s not tracked\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
	  return 1;
  }

  fclose(findex);
  fclose(fnewindex);
  fs_mv(".beargit/.newindex", ".beargit/.index");
  return 0;
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the homework 1 spec.
 *
 */

const char* go_bears = "GO BEARS!";

int is_commit_msg_ok(const char* msg) {
  /* COMPLETE THE REST */
  if(strlen(msg) < strlen(go_bears)){
	return 0;
  }

  for(int i = 0; i <= strlen(msg) - strlen(go_bears); i++){
	int same = 1;
	for(int j = 0; j < strlen(go_bears); j++){
	  if(*(go_bears+j) != *(msg+i+j)) same = 0;
	}
	if(same) return 1;
  }
  return 0;
}

void next_commit_id(char* commit_id) {
  /* COMPLETE THE REST */
 unsigned int i = 0;

  while(commit_id[i] != '\0'){

    switch (commit_id[i]) {
      case '0':
      case 'c':
        commit_id[i] = '6';
        break;
      case '6':
        commit_id[i] = '1';
        goto End;
        break;
      case '1':
        commit_id[i] = 'c';
        goto End;
        break;
      default:
        fprintf(stderr, "ERROR: commit id only contain charactors in ['6', '1', 'c'], origin commit id \"%s\"\n", commit_id);
        break;
    }
    
    i++;
  }

End: return;
}

int beargit_commit(const char* msg) {
  if (!is_commit_msg_ok(msg)) {
    fprintf(stderr, "ERROR: Message must contain \"%s\"\n", go_bears);
    return 1;
  }

  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  next_commit_id(commit_id);

  /* COMPLETE THE REST */
  char com_dir[FILENAME_SIZE]; 
  char com_file[FILENAME_SIZE];

  sprintf(com_dir, ".beargit/%s", commit_id);
  fs_mkdir(com_dir);
  sprintf(com_file, "%s/.index", com_dir);
  fs_cp(".beargit/.index", com_file);
  sprintf(com_file, "%s/.prev", com_dir);
  fs_cp(".beargit/.prev", com_file);
  
  FILE* findex = fopen(".beargit/.index", "r");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    sprintf(com_file, "%s/%s", com_dir, line);
	fs_cp(line, com_file);
  }
  fclose(findex);
  sprintf(com_file, "%s/.msg", com_dir);
  write_string_to_file(com_file, msg);
  write_string_to_file(".beargit/.prev", commit_id);

  return 0;
}

/* beargit status
 *
 * See "Step 1" in the homework 1 spec.
 *
 */

int beargit_status() {
  /* COMPLETE THE REST */
  FILE* findex = fopen(".beargit/.index", "r");

  fprintf(stdout, "Tracked files:\n\n");
  char line[FILENAME_SIZE];
  int count = 0;
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
	count++;
    fprintf(stdout, "  %s\n", line);
  }
  fprintf(stdout, "\n%d files total\n", count);
  fclose(findex);
  return 0;
}

/* beargit log
 *
 * See "Step 4" in the homework 1 spec.
 *
 */

int beargit_log() {
  /* COMPLETE THE REST */

  char dir_name[FILENAME_SIZE];
  char file_name[FILENAME_SIZE];
  char commit_id[COMMIT_ID_SIZE];
  char msg[MSG_SIZE];

  int at_least_one_commit = 0;

  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  while(strcmp(commit_id, "0000000000000000000000000000000000000000") != 0){
	at_least_one_commit++;
	sprintf(dir_name, ".beargit/%s", commit_id);
	fprintf(stdout, "\ncommit %s", commit_id);
	sprintf(file_name, "%s/.msg", dir_name);
	read_string_from_file(file_name, msg, MSG_SIZE);
	fprintf(stdout, "\n    %s\n", msg);
    sprintf(file_name, "%s/.prev", dir_name);
    read_string_from_file(file_name, commit_id, COMMIT_ID_SIZE);
  }
  if(at_least_one_commit){
	printf("\n");
  }
  else{
	fprintf(stderr, "ERROR: There are no commits!\n");
    return 1;
  }

  return 0;
}
