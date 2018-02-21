// author polikuo 2018
// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "editor4sh.h"
struct syntax VARS; 
const char *SHELL = ""; // = "#!/bin/sh\n";
char filename[FL_PATH_MAX], title[FL_PATH_MAX]; 
char search_string[2048]; 
char replace_string[2048]; 
bool color = false, auto_indent = false, indenting = false, changed = false, is_bash = false; 
Fl_Text_Buffer *buff = new Fl_Text_Buffer(); 
Fl_Text_Buffer *stylebuf = new Fl_Text_Buffer(); 
#define TS 14
// Style table
// http://www.fltk.org/doc-1.3/Enumerations_8H.html
// http://www.fltk.org/doc-1.3/fltk-colormap.png
// A - Plain
// B - busybox
// C - special_characters
// D - bourne_function
// E - command substitution
// F - Single Quotes
// G - Double Quotes
// H - escapes
// I - bourne_variables
// J - comments
// K - bourne_constructs
Fl_Text_Display::Style_Table_Entry styletable[] = {
  { FL_BLACK, FL_COURIER, TS },
  { 82, FL_COURIER_BOLD, TS },
  { FL_DARK_MAGENTA, FL_COURIER_BOLD,TS },
  { 92, FL_COURIER_BOLD, TS },
  { FL_DARK_GREEN, FL_COURIER, TS },
  { FL_BLUE, FL_COURIER_BOLD, TS },
  { FL_DARK_CYAN, FL_COURIER,TS },
  { FL_DARK_YELLOW, FL_COURIER,TS },
  { FL_DARK_RED, FL_COURIER_BOLD, TS },
  { FL_DARK2, FL_COURIER_ITALIC, TS },
  { FL_MAGENTA, FL_COURIER_BOLD, TS }
}; 

void buffer_init() {
  // attach buffer to editor
  buff->text(SHELL);
  edit->buffer(buff);
  edit->insert_position(strlen(SHELL));
  edit->show_insert_position();
  buff->add_modify_callback(modification_cb, edit);
  // associate style & buffer
  edit->highlight_data(
    stylebuf,
    styletable,
    sizeof(styletable) / sizeof(styletable[0]),
    'A',
    style_unfinished_cb,
    0 // void*
  );
  // buff->add_modify_callback(style_update, edit);
  // buff->add_modify_callback(modification_cb, edit);
}

void chk_bash() {
  if (buff->char_at(0) == 35 && buff->char_at(1) == 33) {
// boolean is_bash
// check for shebang "#!"
    int firstline = buff->line_end(0);
    // either a pointer to the newline character ending the line
    // or a pointer to one character beyond the end of the buffer
    char *shell;
    shell = buff->text_range(firstline - 4, firstline);
    if (strcmp(shell, "bash") == 0) is_bash = true;
    else is_bash = false;
    free(shell);
    return;
  }
  // default value
  is_bash = false;
}

void shell_cb(int choice) {
  if (choice == 1) {
    SHELL = "#!/bin/sh\n";
    chk_bash();
    if (is_bash) {
      is_bash = false;
      shebang(1);
    } else shebang(0);
    return;
  }
  if (choice == 2) {
    SHELL = "#!/usr/bin/env bash\n";
    chk_bash();
    if (is_bash) shebang(0);
    else {
      is_bash = true;
      shebang(1);
    }
    return;
  }
  SHELL = "";
  chk_bash();
  if (is_bash) {
    is_bash = false;
    shebang(1);
  } else shebang(0);
}

void shebang(int restyle) {
  int shell_len = strlen(SHELL);
  if (shell_len == 0) {
    if (restyle && color) stylebuf_init();
    return;
  }
  int firstline, current_pos;
  char style[21];
  current_pos = edit->insert_position();
  
  if (color && !restyle) {
    memset(style, 'J', shell_len - 1);
    style[shell_len - 1] = 10; // '\n'
    style[shell_len] = 0; // '\0'
  }
  
  // check for shebang "#!"
  if (buff->char_at(0) == 35 && buff->char_at(1) == 33) {
    // search_forward(int startPos, const char *searchString, int *foundPos, int matchCase = 0)
    int found = buff->search_forward(0, "\n", &firstline);
    if (found) {
      buff->replace(0, firstline + 1, SHELL);
      if (color && !restyle) stylebuf->replace(0, firstline + 1, style);
      edit->insert_position(current_pos + shell_len - firstline - 1);
    } else {
      // no newline char
      buff->text(SHELL);
      if (color) stylebuf->text(style);
      edit->insert_position(shell_len);
    }
  } else {
    buff->insert(0, SHELL);
    if (color && !restyle) stylebuf->insert(0, style);
    edit->insert_position(current_pos + shell_len);
  }
  edit->show_insert_position();
  // restyle, "is_bash" has been changed
  if (restyle && color) stylebuf_init();
}

int check_saved() {
  // 1:good, 0:cancel
  if (!changed) return 1;
  int r = fl_choice(
    "The current file has not been saved.\nWould you like to save it now?",
    "Cancel", "Save", "Don't Save"
  );
  // Cancel = 0, Save = 1, Don't Save = 2
  if (r == 1) {
    save_cb(); // Save the file...
    return !changed;
  }
  // if (r == 2) changed = false;
  return r;
}

void new_cb() {
  if (!check_saved()) return;
  if (SHELL) {
    buff->text(SHELL);
    // coloring
    if (color) stylebuf_init();
  }
  /*
  if (color) {
    color_cb(); // called twice
    buff->text(SHELL);
    color_cb(); // to refresh
  } else {
    buff->text(SHELL);
  }
  */
  edit->insert_position(strlen(SHELL));
  edit->show_insert_position();
  strcpy(filename, "");
  strcpy(title, "");
  win->label(title);
  changed = false;
}

void save_file(const char *newfile) {
  // Save file
  // auto append trailing "\n"
  int buff_end;
  buff_end = buff->length();
  char last_char;
  last_char = buff->char_at(buff_end - 1);
  if (last_char != 10) {
    buff->append("\n");
    edit->insert_position(buff_end + 1);
    edit->show_insert_position();
  }
  // fl_alert("Saving to file %s", newfile);
  if (buff->savefile(newfile)) {
    fl_alert("Error writing to file \'%s\':\n%s.", newfile, strerror(errno));
  } else {
    strcpy(filename, newfile);
    strcpy(title, newfile);
    win->label(title);
    changed = false;
    #ifdef __linux__
      char chmod[FL_PATH_MAX + 9];
      sprintf(chmod, "chmod +x %s", newfile);
      system(chmod);
    #endif
  }
}

void saveas_cb() {
  char *sfa;
  sfa = fl_file_chooser("Save File As ?", "*", filename);
  if(sfa != NULL) save_file(sfa);
}

void save_cb() {
  if (filename[0] == 0) {
    // No filename
    saveas_cb();
  } else {
    save_file(filename);
  }
}

void open_cb() {
  if (!check_saved()) return;
  char *newfile = fl_file_chooser("Open File ?", "*", filename);
  if (newfile != NULL) load_file(newfile);
}

void load_file(char *newfile) {
  bool color_resume = color;
  // disable color before loading
  if (color_resume) {
    color_cb();
  }
  int lf;
  lf = buff->loadfile(newfile);
  if (lf) {
    fl_alert("Error while loading file \'%s\':\n%s.", newfile, strerror(errno));
  } else {
    strcpy(filename, newfile);
    strcpy(title, newfile);
    win->label(title);
    changed = false;
  }
  // is_bash
  // check for shebang "#!"
  if (buff->char_at(0) == 35 && buff->char_at(1) == 33) {
    int firstline = buff->line_end(0);
    // either a pointer to the newline character ending the line
    // or a pointer to one character beyond the end of the buffer
    char *shell;
    shell = buff->text_range(firstline - 4, firstline);
    if (strcmp(shell, "bash") == 0) is_bash = true;
    else is_bash = false;
    free(shell);
  }
  if (color_resume) color_cb();
}

void style_unfinished_cb(int, void*) {
// call back for highlight data
// if style 'A' is found,
// this call back will be called.
// void *cbArg will be passed as arguement
// fl_alert("style_unfinished_cb");
}

int busybox(char *textbuff, char *scanned_text) {
  return compare_keywords(
    textbuff,
    applets,
    sizeof(applets)/sizeof(*applets),
    scanned_text,
    66
  );
}

int bourne_builtins(char *textbuff, char *scanned_text) {
// compare_keywords(
//   address_to_buff,
//   array,
//   sizeof_array,
//   address_to_result,
//   color_style(A~K)
// )
  if (is_bash) {
    int bourne_again;
    bourne_again = compare_keywords(
      textbuff,
      bourne_again_function,
      sizeof(bourne_again_function)/sizeof(*bourne_again_function),
      scanned_text,
      68
    );
    if (bourne_again == 0) {
      bourne_again = compare_keywords(
        textbuff,
        bourne_again_keys,
        sizeof(bourne_again_keys)/sizeof(*bourne_again_keys),
        scanned_text,
        75
      );
    }
    return bourne_again;
  }
  int bourne;
  bourne = compare_keywords(
    textbuff,
    bourne_function,
    sizeof(bourne_function)/sizeof(*bourne_function),
    scanned_text,
    68
  );
  if (bourne == 0) {
    bourne = compare_keywords(
      textbuff,
      bourne_keys,
      sizeof(bourne_keys)/sizeof(*bourne_keys),
      scanned_text,
      75
    );
  }
  return bourne;
}

int is_special(char ascii) {
  // 33~47, 58~64, 91~96, 123~126
  if (ascii == 95) {
    // 95 _ usually treated as normal string
    // only exception $_
    // already handled
    return 0;
  }
  if (ascii > 32 && ascii < 48) {
    return 1;
  }
  if (ascii > 57 && ascii < 65) {
    return 2;
  }
  if (ascii > 90 && ascii < 97) {
    return 3;
  }
  if (ascii > 122 && ascii < 127) {
    return 4;
  }
  return 0;
}

void disable_color() {
  // buff->remove_modify_callback(modification_cb, edit);
  // hopefully disable the style thing
  stylebuf->text("");
  // edit->highlight_data(NULL, NULL, NULL, NULL, NULL);
}

void auto_indent_switch() {
  auto_indent = !auto_indent;
  if (auto_indent) {
    indent_switch->label("No  Indent");
  } else {
    indent_switch->label("AutoIndent");
  }
}

void auto_indent_cb(int lsp, int pos, char *line) {
// lsp: line start pos
// pos: insert position
// line: entire line

  size_t current_pos = pos + 1; // '\n'
  // '\n'indent_space'\0'
  char *indention = (char *)malloc(current_pos - lsp); // pos - lsp + 1
  
  size_t ai = 0;
  for (;ai < (current_pos - lsp);ai++) {
    // tab || space
    if (line[ai] == 9 || line[ai] == 32) indention[ai] = line[ai];
    else break;
  }
  if (ai == 0) {
    /*
    if (color) {
      indention[0] = 10; // '\n'
      indention[0] = 0; // '\0'
      stylebuf->replace(current_pos, current_pos, indention);
    }
    */
    free(indention);
    free(line);
    edit->insert_position(current_pos - 1);
    return;
  }
  indention[ai] = 0; // '\0'
  buff->insert(current_pos, indention); // will call modification_cb
  // stylebuf is one char slower than buff
  current_pos += strlen(indention);
  if (color) {
    // modification_cb already handle the inserted '\n', using "replace"
    stylebuf->replace(pos + 1, current_pos, indention);
  }
  free(indention);
  free(line);
  edit->insert_position(current_pos - 1);
}

void stylebuf_init() {
  // VARS init
  VARS.pssp = 0;
  VARS.cm = false;
  VARS.es = false;
  VARS.ds = false;
  VARS.se = false;
  VARS.bq = false;
  VARS.sq = false;
  VARS.dq = false;
  VARS.pssp = 0;
  VARS.cs = 0;
  VARS.p = 0;
  // buff_copy
  int bufflen;
  char *buff_copy;
  buff_copy = buff->text(); // full copy
  if (buff_copy) {
    bufflen = strlen(buff_copy); // end_pos + 1
  } else {
    fl_alert("*** Error, malloc failed ***\nDisabling the coloring!");
    color_cb();
    return;
  }
  scan_forward(buff_copy, bufflen, 0);
  free(buff_copy);
  return;
}

void color_cb() {
  color = !color;
  if (color) {
    stylebuf_init();
    color_switch->label("NoColor");
  } else {
    disable_color();
    color_switch->label("Colorful");
  }
  edit->redisplay_range(0, buff->length());
}

int compare_keywords(char *text, const char *keys[], size_t elements, char *result, char ascii) {
  const char *needle;
  size_t nl; // needle length
  size_t k = 0;
  size_t mp; // matching point
  for (; k < elements; k++) {
    needle = keys[k];
    nl = strlen(needle);
    // matching first char
    if (*text == *needle) {
      // check posterior
      switch (text[nl]) {
        case 0: // end of string
        case 9: // tab
        case 10: // newline
        case 32: // space
        case 38: // &
        case 41: // )
        case 59: // ;
        case 60: // <
        case 62: // >
        case 96: // `
        case 124: // |
          break;
        default:
          // next keyword
          continue;
      }
      for (mp = 1; mp < nl; mp++) {
        // matching
        if (text[mp] != needle[mp]) {
          // next element
          break;
        }
      }
      if (mp == nl) {
        // matched
        break;
      }
    } else {
      continue;
    }
  }
  if (k != elements) {
    while (*needle++) {
      *result = ascii;
      result++;
    }
    return nl - 1;
  }
  return 0;
}

void setvars(char *buff_copy, int pos) {
  // VARS init
  VARS.cm = false;
  VARS.es = false;
  VARS.ds = false;
  VARS.se = false;
  VARS.bq = false;
  VARS.sq = false;
  VARS.dq = false;
  VARS.cs = 0;
  VARS.p = 0;
  int setvar = 0;
  
  // [[fallthrough]]; --> suppress the fallthrough warning
  
  for (;setvar < pos; setvar++) {
    switch(buff_copy[setvar]) {
      case 10: // newline
        if (VARS.cm) VARS.cm = !VARS.cm;
        [[fallthrough]];
      case 9: // tab
        [[fallthrough]];
      case 32: // space
        if (VARS.ds) VARS.ds = !VARS.ds;
        continue;
      case 34: // "double quote"
        if (VARS.cm) break;
        if (!VARS.sq) {
          VARS.dq = !VARS.dq;
          if (VARS.ds) VARS.ds = !VARS.ds;
          continue;
        }
        break;
      case 35: // #comment
        if (!VARS.ds && !VARS.sq && !VARS.dq && !VARS.se) {
          VARS.cm = true;
        }
        break;
      case 36: // $dolor_sign
        if (VARS.cm || VARS.sq) break;
        VARS.ds = true;
        switch(buff_copy[setvar+1]) {
          case 33: // built-in variable $!
          case 35: // built-in variable $#
          case 36: // built-in variable $$
          case 42: // built-in variable $*
          case 45: // built-in variable $-
          case 63: // built-in variable $?
          case 64: // built-in variable $@
          case 95: // built-in variable $_
            setvar++;
            VARS.ds = false;
            continue;
          case 40: // $(...
            VARS.cs++;
            VARS.p++;
            setvar++;
            VARS.ds = false;
            continue;
          case 123: // shell expansion ${
            // cb++;
            VARS.se = true; // a bit redundant
            setvar++;
            VARS.ds = true;
            continue;
          case 0: // '\0'
          case 9: // tab
          case 10: // newline
          case 32: // space
          case 47: // regular expression /$/
            // trailing dolor_sign$
            VARS.ds = false;
            // resume default
            continue;
        }
        if (is_special(buff_copy[setvar+1])) {
          // invalid variable $, or such
          // resume default
          VARS.ds = false;
        }
        break;
      case 39: // 'single quote'
        if (VARS.cm) break;
        // single quote is always
        // treated literally in double quote
        if (!VARS.dq) {
          VARS.sq = !VARS.sq;
          continue;
        }
        break;
      case 40: // (
        if (VARS.cm || VARS.sq || VARS.dq) break;
        VARS.p++;
        break;
      case 41: // )
        if (VARS.cm || VARS.sq) break;
        if (VARS.ds) VARS.ds = !VARS.ds;
        if (VARS.dq && VARS.cs == 0) break;
        VARS.p--;
        break;
      case 92: // \escapes
        if (!VARS.cm && !VARS.sq) VARS.es = true;
        break; // other escape
      case 96: // `back quote`
        if (VARS.cm || VARS.sq) break;
        VARS.ds = false;
        VARS.bq = !VARS.bq;
        continue;
      case 125: // }
        if (VARS.cm || VARS.sq) break;
        if (VARS.ds || VARS.se) {
          // end of variable
          VARS.ds = false;
          VARS.se = false;
          continue;
        }
        break;
    }
    if (VARS.se) continue;
    if (VARS.cm) {
      if (buff_copy[setvar] == 10) VARS.cm = false;
      continue;
    }
    if (VARS.sq) {
      // 'single quote' ignore escapes
      if (buff_copy[setvar] == 39) VARS.sq = !VARS.sq;
      continue;
    }
    if (VARS.ds) {
      if (is_special(buff_copy[setvar+1]) || buff_copy[setvar+1] == 0) {
        // end of variable
        VARS.ds = false;
        continue;
      }
    }
    if (VARS.es) {
      // this char = backslash (92)
      if (VARS.dq && buff_copy[setvar+1] == 39) {
        // echo "\'" --> \'
        VARS.es = false;
        continue;
      }
      if (buff_copy[setvar+1] == 120) {
        // hex \xFF 48~57, 65~70, 97~102
        bool hex = ((
          (buff_copy[setvar+2] > 47 && buff_copy[setvar+2] < 58) ||
          (buff_copy[setvar+2] > 64 && buff_copy[setvar+2] < 71) ||
          (buff_copy[setvar+2] > 96 && buff_copy[setvar+2] < 103)
        ) && (
          (buff_copy[setvar+3] > 47 && buff_copy[setvar+3] < 58) ||
          (buff_copy[setvar+3] > 64 && buff_copy[setvar+3] < 71) ||
          (buff_copy[setvar+3] > 96 && buff_copy[setvar+3] < 103)
        ));
        if (hex) setvar+=2;
      }
      setvar++;
      VARS.es = false;
      continue;
    }
    if (VARS.cs > 0) {
      if (buff_copy[setvar] == 41) {
        // )
        VARS.cs--;
        if (VARS.p > VARS.cs) VARS.cs++;
      }
      // continue;
    }
  }
}

void scan_forward(char *buff_copy, int bufflen, int pos) {
  char *scan;
  scan = (char*)malloc(bufflen - pos + 1);
  if (scan) {
    memset(scan, 65, bufflen - pos); // 'A'
    scan[bufflen - pos] = 0; // '\0'
  } else {
    fl_alert("*** Error, malloc failed ***\nDisabling the coloring!");
    color_cb();
    return;
  }
  struct syntax UPDATE;
  // duplicate the struct to preserve VARS values
  UPDATE = VARS;
  int j = 0;
  int m = 0;
  bool key = true;
  
  // [[fallthrough]]; --> suppress the fallthrough warning
  
  for (;j < bufflen - pos; j++) {
    switch(buff_copy[j+pos]) {
      case 10: // newline
        if (UPDATE.cm) UPDATE.cm = !UPDATE.cm;
        [[fallthrough]];
      case 9: // tab
        [[fallthrough]];
      case 32: // space
        key = true;
        if (UPDATE.ds) UPDATE.ds = !UPDATE.ds;
        scan[j] = buff_copy[j+pos];
        continue;
      case 34: // "double quote"
        if (UPDATE.cm) break;
        if (!UPDATE.sq) {
          UPDATE.dq = !UPDATE.dq;
          scan[j] = 'G';
          if (UPDATE.ds) UPDATE.ds = !UPDATE.ds;
          continue;
        }
        break;
      case 35: // #comment
        key = false;
        if (!UPDATE.ds && !UPDATE.sq && !UPDATE.dq && !UPDATE.se) {
          UPDATE.cm = true;
          scan[j] = 'J';
        }
        break;
      case 36: // $dolor_sign
        key = false;
        if (UPDATE.cm || UPDATE.sq) break;
        UPDATE.ds = true;
        switch(buff_copy[j+pos+1]) {
          case 33: // built-in variable $!
          case 35: // built-in variable $#
          case 36: // built-in variable $$
          case 42: // built-in variable $*
          case 45: // built-in variable $-
          case 63: // built-in variable $?
          case 64: // built-in variable $@
          case 95: // built-in variable $_
            scan[j] = 'I';
            j++;
            scan[j] = 'I';
            UPDATE.ds = false;
            continue;
          case 40: // $(...
            UPDATE.cs++;
            UPDATE.p++;
            scan[j] = 'E';
            j++;
            scan[j] = 'E';
            key = true;
            UPDATE.ds = false;
            continue;
          case 123: // shell expansion ${
            // cb++;
            UPDATE.se = true; // a bit redundant
            scan[j] = 'I';
            j++;
            scan[j] = 'I';
            UPDATE.ds = true;
            continue;
          case 0: // '\0'
          case 9: // tab
          case 10: // newline
          case 32: // space
          case 47: // regular expression /$/
            // trailing dolor_sign$
            UPDATE.ds = false;
            // resume default
            scan[j] = 'A';
            continue;
        }
        scan[j] = 'I';
        if (is_special(buff_copy[j+pos+1])) {
          // invalid variable $, or such
          // resume default
          UPDATE.ds = false;
          scan[j] = 'A';
        }
        break;
      case 39: // 'single quote'
        key = false;
        if (UPDATE.cm) break;
        // single quote is always
        // treated literally in double quote
        if (!UPDATE.dq) {
          UPDATE.sq = !UPDATE.sq;
          scan[j] = 'F';
          continue;
        }
        break;
      case 40: // (
        key = true;
        if (UPDATE.cm || UPDATE.sq || UPDATE.dq) break;
        UPDATE.p++;
        scan[j] = 'C';
        break;
      case 41: // )
        key = false;
        if (UPDATE.cm || UPDATE.sq) break;
        if (UPDATE.ds) UPDATE.ds = !UPDATE.ds;
        if (UPDATE.dq) {
          if (UPDATE.cs == 0)
          break;
        }
        UPDATE.p--;
        scan[j] = 'C';
        break;
      case 92: // \escapes
        key = false;
        if (!UPDATE.cm && !UPDATE.sq) UPDATE.es = true;
        break; // other escape
      case 96: // `back quote`
        key = true;
        if (UPDATE.cm || UPDATE.sq) break;
        scan[j] = 'E';
        UPDATE.ds = false;
        UPDATE.bq = !UPDATE.bq;
        continue;
      case 123: // {
        scan[j] = 'C';
        break;
      case 125: // }
        if (UPDATE.cm || UPDATE.sq) break;
        if (UPDATE.ds || UPDATE.se) {
          // end of variable
          UPDATE.ds = false;
          UPDATE.se = false;
          scan[j] = 'I';
          continue;
        }
        scan[j] = 'C';
        break;
      case 38: // &
      case 59: // ;
      case 60: // <
      case 62: // >
      case 124: // >
        // 38, 59, 60, 62, 124 fall through to default
        key = true;
        [[fallthrough]];
      default:
        if (UPDATE.cm || UPDATE.es || UPDATE.sq) break;
        // scan[j] = 'A'; --> overwrite the keywords
        if (is_special(buff_copy[j+pos])) scan[j] = 'C';
    }
    if (UPDATE.se) {
      scan[j] = 'I';
      continue;
    }
    if (UPDATE.cm) {
      switch (buff_copy[j+pos]) {
        case 10: // newline
          UPDATE.cm = false;
          [[fallthrough]];
        case 9: // tab
        case 32: // space
          scan[j] = buff_copy[j+pos];
          break;
        default:
          scan[j] = 'J';
      }
      continue;
    }
    if (UPDATE.sq) {
      // 'single quote' ignore escapes
      scan[j] = 'F';
      if (buff_copy[j+pos] == 39) UPDATE.sq = !UPDATE.sq;
      continue;
    }
    if (UPDATE.dq) scan[j] = 'G';
    if (UPDATE.ds) {
      scan[j] = 'I';
      if (is_special(buff_copy[j+pos+1]) || buff_copy[j+pos+1] == 0) {
        // end of variable
        UPDATE.ds = false;
        continue;
      }
    }
    if (UPDATE.es) {
      // this char = backslash (92)
      if (buff_copy[j+pos+1]) {
        // char not null
        scan[j] = 'H';
        scan[j+1] = 'H';
      }
      if (UPDATE.dq && buff_copy[j+pos+1] == 39) {
        // echo "\'" --> \'
        UPDATE.es = false;
        continue;
      }
      if (buff_copy[j+pos+1] == 120) {
        // hex \xFF 48~57, 65~70, 97~102
        bool hex = ((
          (buff_copy[j+pos+2] > 47 && buff_copy[j+pos+2] < 58) ||
          (buff_copy[j+pos+2] > 64 && buff_copy[j+pos+2] < 71) ||
          (buff_copy[j+pos+2] > 96 && buff_copy[j+pos+2] < 103)
        ) && (
          (buff_copy[j+pos+3] > 47 && buff_copy[j+pos+3] < 58) ||
          (buff_copy[j+pos+3] > 64 && buff_copy[j+pos+3] < 71) ||
          (buff_copy[j+pos+3] > 96 && buff_copy[j+pos+3] < 103)
        ));
        if (hex) {
          scan[j+2] = 'H';
          scan[j+3] = 'H';
          j+=2;
        }
      }
      j++;
      UPDATE.es = false;
      continue;
    }
    if (UPDATE.cs > 0) {
      if (scan[j] == 'A') scan[j] = 'E';
      if (buff_copy[j+pos] == 41) {
        // )
        scan[j] = 'E';
        UPDATE.cs--;
        if (UPDATE.p > UPDATE.cs) UPDATE.cs++;
      }
      // continue;
    }
    if (UPDATE.bq) {
      if (scan[j] == 'A') scan[j] = 'E';
      // continue;
    }
    if (key) {
      // scan for keywords
      key = false;
      if (UPDATE.dq) {
        if (UPDATE.cs == 0 && !UPDATE.bq)
        continue;
      }
      // when keywords are overlapped, the "bourne_builtins" one is used
      m = bourne_builtins(&buff_copy[j+pos], &scan[j]);
      if (m == 0) m = busybox(&buff_copy[j+pos], &scan[j]);
      j += m;
    }
  }
  stylebuf->replace(pos, stylebuf->length(), scan);
  /*
  replace example:
  {'s', 't', 'r', 'i', 'n', 'g'}
  replace(1, 3, "ABC")
  {'s', 'A', 'B', 'C', 'i', 'n', 'g'}
  */
  edit->redisplay_range(pos, buff->length());
  free(scan);
  return;
}

void modification_cb(int pos, int nInserted, int nDeleted, int nRestyled, const char *deletedText, void *cbArg) {
  if (!changed) {
    if (nInserted != 0 || nDeleted != 0) {
      // adjust title
      changed = true;
      strcat(title, "*");
      win->label(title);
    }
  }
  if (nInserted > 0 && nDeleted > 0) {
    // This only happens when a user undo changes
    // do nothing...
    return;
  }
  if (auto_indent && nInserted == 1 && buff->char_at(pos) == 10) {
    indenting = true;
    auto_indent_cb(
      buff->line_start(pos),
      pos, // insert position
      buff->line_text(pos)
    );
    return;
  }
  // int scan_pos
  int scan_pos;
  // scan from the start of the line to check for key words
  scan_pos = buff->line_start(pos);
  // buff_copy
  int bufflen;
  char *buff_copy;
  buff_copy = buff->text(); // full copy
  if (buff_copy) {
    bufflen = strlen(buff_copy); // end_pos + 1
  } else {
    fl_alert("*** Error, malloc failed ***\nDisabling the coloring!");
    color_cb();
    return;
  }
  if (nInserted == 0 && nDeleted == 0) {
    if (color) {
      setvars(buff_copy, scan_pos);
    }
    // selection change
    stylebuf->unselect();
    free(buff_copy);
    return;
  }
  if (color) {
    if (indenting) {
      indenting = false;
      scan_pos = buff->line_start(scan_pos - 2);
    }
    if (VARS.pssp != scan_pos || indenting) {
      setvars(buff_copy, scan_pos);
    }
    scan_forward(buff_copy, bufflen, scan_pos);
    // fl_alert("%d\n", scan_pos);
    free(buff_copy);
    VARS.pssp = scan_pos;
    if (buff->length() != stylebuf->length()) {
      // catch exception
      fl_alert(
        "!!!Exception occur!!!\nbuff->length = %d\nstylebuf->length = %d", // \nForcing reload...",
        buff->length(),
        stylebuf->length()
      );
      /*
      color_cb();
      color_cb();
      */
      // Debug
      fl_alert("pos:%d\nnInserted:%d\nnDeleted:%d\nnRestyled:%d\ndeletedText:\n%s",
      pos, nInserted, nDeleted, nRestyled, deletedText);
      fl_alert("current buffer: '%c'\ncurrent style: '%c'",
      buff->char_at(pos), stylebuf->char_at(pos));
      fl_alert("current buffer: '%d'\ncurrent style: '%d'",
      buff->char_at(pos), stylebuf->char_at(pos));
    }
  }
}

void close_cb(Fl_Widget*, void* v) {
  if (check_saved()) exit(0);
}

int arg_parser(int argc, char **argv, int &i) {
  if(strcmp(argv[i], "-shell") == 0) {
    SHELL = "#!/bin/sh\n";
    i++;
    return 1;
  }
  if(strcmp(argv[i], "-bash") == 0) {
    SHELL = "#!/usr/bin/env bash\n";
    i++;
    return 1;
  }
  if(strcmp(argv[i], "-other") == 0) {
    SHELL = 0;
    i++;
    return 1;
  }
  if(strcmp(argv[i], "-h") == 0) {
    printf(
      "%s [-options] [file]\n"
      "Set default shebang to\n"
      "-shell\t#!/bin/sh\n"
      "-bash\t#!/usr/bin/env bash\n"
      "-other\tother unknown shebang\n"
      "The later option will replace any previous one(s)\n"
      "Any arguements after the \"file\" will be ignored\n"
      , basename(argv[0])
    );
    exit(0);
  }
  return 0;
// http://www.fltk.org/articles.php?L744
}

void find_dialog() {
  const char *SS = fl_input("Search String:", search_string);
  // Search string is blank
  if (SS == NULL || *SS == 0) return;
  int sf_pos;
  int sf_r = buff->search_forward(
    edit->insert_position(),
    SS,
    &sf_pos,
    0
  );
  if (sf_r) {
    buff->select(sf_pos, sf_pos + strlen(SS));
    edit->insert_position(sf_pos + 1);
    edit->show_insert_position();
  } else {
    fl_alert("No occurrences of \'%s\' found!", SS);
  }
  strcpy(search_string, SS);
}

void find_again_dialog() {
  if (*search_string == 0) {
    // Search string is blank
    find_dialog();
    return;
  }
  int sf_pos;
  int sf_r = buff->search_forward(
    edit->insert_position(),
    search_string,
    &sf_pos,
    0
  );
  if (sf_r) {
    buff->select(sf_pos, sf_pos + strlen(search_string));
    edit->insert_position(sf_pos + 1);
    edit->show_insert_position();
  } else {
    fl_alert("No occurrences of \'%s\' found!", search_string);
  }
}

Fl_Double_Window *replace_dialog_window=(Fl_Double_Window *)0;

Fl_Input *text_in_find=(Fl_Input *)0;

Fl_Input *text_in_replace=(Fl_Input *)0;

Fl_Button *replace_all_btn=(Fl_Button *)0;

static void cb_replace_all_btn(Fl_Button*, void*) {
  replace_all_cb();
}

Fl_Return_Button *replace_next_btn=(Fl_Return_Button *)0;

static void cb_replace_next_btn(Fl_Return_Button*, void*) {
  replace_next_cb();
}

Fl_Button *RD_cancel_btn=(Fl_Button *)0;

static void cb_RD_cancel_btn(Fl_Button*, void*) {
  close_dialog(replace_dialog_window, 0);
}

Fl_Double_Window* replace_dialog() {
  { replace_dialog_window = new Fl_Double_Window(320, 155, "Replace");
    { text_in_find = new Fl_Input(72, 20, 230, 30, "Find:");
      text_in_find->box(FL_PLASTIC_DOWN_BOX);
    } // Fl_Input* text_in_find
    { text_in_replace = new Fl_Input(72, 60, 230, 30, "Replace:");
      text_in_replace->box(FL_PLASTIC_DOWN_BOX);
    } // Fl_Input* text_in_replace
    { replace_all_btn = new Fl_Button(15, 110, 85, 30, "Replace All");
      replace_all_btn->box(FL_PLASTIC_UP_BOX);
      replace_all_btn->down_box(FL_PLASTIC_DOWN_BOX);
      replace_all_btn->callback((Fl_Callback*)cb_replace_all_btn);
    } // Fl_Button* replace_all_btn
    { replace_next_btn = new Fl_Return_Button(105, 110, 130, 30, "Replace Next");
      replace_next_btn->box(FL_PLASTIC_UP_BOX);
      replace_next_btn->down_box(FL_PLASTIC_DOWN_BOX);
      replace_next_btn->callback((Fl_Callback*)cb_replace_next_btn);
    } // Fl_Return_Button* replace_next_btn
    { RD_cancel_btn = new Fl_Button(240, 110, 64, 30, "Cancel");
      RD_cancel_btn->box(FL_PLASTIC_UP_BOX);
      RD_cancel_btn->down_box(FL_PLASTIC_DOWN_BOX);
      RD_cancel_btn->callback((Fl_Callback*)cb_RD_cancel_btn);
    } // Fl_Button* RD_cancel_btn
    replace_dialog_window->end();
  } // Fl_Double_Window* replace_dialog_window
  /*
  replace_dialog_window->show();
  const char *find = text_in_find->value();
  const char *replace = text_in_replace->value();
  fl_alert(find);
  fl_alert(replace);
  */
  /*
  delete text_in_find;
  delete text_in_replace;
  delete replace_all_btn;
  delete RD_cancel_btn;
  delete replace_next_btn;
  delete replace_dialog_window;
  */
  text_in_find->value(search_string);
  text_in_replace->value(replace_string);
  replace_dialog_window->callback((Fl_Callback *)close_dialog, win);
  replace_dialog_window->show();
  return replace_dialog_window;
}

void replace_next_cb() {
  const char *TIF = text_in_find->value();
  const char *TIR = text_in_replace->value();
  // text in find is blank
  if (TIF == NULL || *TIF == 0) return;
  strcpy(search_string, TIF);
  strcpy(replace_string, TIR);
  int rpl_pos;
  int rpl_r = buff->search_forward(
    edit->insert_position(),
    TIF,
    &rpl_pos,
    1
  );
  if (rpl_r) {
    buff->remove(rpl_pos, rpl_pos + strlen(TIF));
    buff->insert(rpl_pos, TIR);
    buff->select(rpl_pos, rpl_pos + strlen(TIR));
    edit->insert_position(rpl_pos + 1);
    edit->show_insert_position();
  } else {
    fl_alert("No occurrences of \'%s\' found!", TIF);
  }
  close_dialog(replace_dialog_window, 0);
}

void replace_again_cb() {
  // text in buffer is blank
  if (*search_string == 0) {
    replace_dialog();
    return;
  }
  int rpl_pos;
  int rpl_r = buff->search_forward(
    edit->insert_position(),
    search_string,
    &rpl_pos,
    1
  );
  if (rpl_r) {
    buff->remove(rpl_pos, rpl_pos + strlen(search_string));
    buff->insert(rpl_pos, replace_string);
    buff->select(rpl_pos, rpl_pos + strlen(replace_string));
    edit->insert_position(rpl_pos + 1);
    edit->show_insert_position();
  } else {
    fl_alert("No occurrences of \'%s\' found!", search_string);
  }
}

void replace_all_cb() {
  const char *TIF = text_in_find->value();
  const char *TIR = text_in_replace->value();
  // text in find is blank
  if (TIF == NULL || *TIF == 0) return;
  
  strcpy(search_string, TIF);
  strcpy(replace_string, TIR);
  int current_pos = edit->insert_position();
  edit->insert_position(0);
  
  int counts = 0;
  int found = 1;
  int srh_pos = 0;
  int rpl_pos = 0;
  bool resume = false;
  
  // disable coloring to speed up looping
  if (color) {
    color_cb();
    resume = true;
  }
  
  while (found) {
    found = buff->search_forward(
      srh_pos,
      TIF,
      &rpl_pos,
      1
    );
    if (found) {
      buff->remove(rpl_pos, rpl_pos + strlen(TIF));
      buff->insert(rpl_pos, TIR);
      counts++;
    }
    if (rpl_pos > srh_pos) srh_pos = rpl_pos + 1;
  }
  
  if (resume) color_cb();
  
  if (counts) fl_message("Replaced %d occurrences.", counts);
  else fl_alert("No occurrences of \'%s\' found!", TIF);
  
  close_dialog(replace_dialog_window, 0);
  edit->insert_position(current_pos);
}

void close_dialog(Fl_Widget*, void* v) {
  // fl_alert("Before deleting");
  delete text_in_find;
  delete text_in_replace;
  delete replace_all_btn;
  delete RD_cancel_btn;
  delete replace_next_btn;
  delete replace_dialog_window;
  // fl_alert("After deleting");
}

Fl_Double_Window *win=(Fl_Double_Window *)0;

Fl_Text_Editor *edit=(Fl_Text_Editor *)0;

Fl_Menu_Bar *menu_bar=(Fl_Menu_Bar *)0;

static void cb_new_bt(Fl_Menu_*, void*) {
  new_cb();
}

static void cb_open(Fl_Menu_*, void*) {
  open_cb();
}

static void cb_save(Fl_Menu_*, void*) {
  save_cb();
}

static void cb_saveas(Fl_Menu_*, void*) {
  saveas_cb();
}

static void cb_sh(Fl_Menu_*, void*) {
  shell_cb(1);
}

static void cb_bash(Fl_Menu_*, void*) {
  shell_cb(2);
}

static void cb_other(Fl_Menu_*, void*) {
  shell_cb(0);
}

static void cb_find_btn(Fl_Menu_*, void*) {
  find_dialog();
}

static void cb_find_again_btn(Fl_Menu_*, void*) {
  find_again_dialog();
}

static void cb_replace_btn(Fl_Menu_*, void*) {
  replace_dialog();
}

static void cb_replace_again_btn(Fl_Menu_*, void*) {
  replace_again_cb();
}

static void cb_color_switch(Fl_Menu_*, void*) {
  color_cb();
}

static void cb_indent_switch(Fl_Menu_*, void*) {
  auto_indent_switch();
}

Fl_Menu_Item menu_menu_bar[] = {
 {"File", 0,  0, 0, 64, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"&New File", 0x4006e,  (Fl_Callback*)cb_new_bt, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"&Open", 0x4006f,  (Fl_Callback*)cb_open, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"&Save", 0x40073,  (Fl_Callback*)cb_save, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"Save &As", 0x50073,  (Fl_Callback*)cb_saveas, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Shell", 0,  0, 0, 64, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"POSIX sh", 0x80073,  (Fl_Callback*)cb_sh, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"ENV bash", 0x80062,  (Fl_Callback*)cb_bash, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"Other", 0,  (Fl_Callback*)cb_other, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Search", 0,  0, 0, 64, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"&Find", 0x40066,  (Fl_Callback*)cb_find_btn, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"Find A&gain", 0x40067,  (Fl_Callback*)cb_find_again_btn, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"&Replace", 0x40072,  (Fl_Callback*)cb_replace_btn, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"Replace &Again", 0x40074,  (Fl_Callback*)cb_replace_again_btn, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Colorful", 0,  (Fl_Callback*)cb_color_switch, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"Auto Indent", 0,  (Fl_Callback*)cb_indent_switch, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

int main(int argc, char **argv) {
  { win = new Fl_Double_Window(425, 320);
    win->box(FL_PLASTIC_UP_BOX);
    { edit = new Fl_Text_Editor(1, 29, 424, 288);
      edit->box(FL_PLASTIC_DOWN_FRAME);
      edit->textfont(4);
      edit->textsize(20);
      Fl_Group::current()->resizable(edit);
    } // Fl_Text_Editor* edit
    { menu_bar = new Fl_Menu_Bar(0, 0, 425, 30);
      menu_bar->box(FL_PLASTIC_UP_BOX);
      menu_bar->down_box(FL_PLASTIC_DOWN_BOX);
      menu_bar->menu(menu_menu_bar);
    } // Fl_Menu_Bar* menu_bar
    win->end();
  } // Fl_Double_Window* win
  // arguements
  int arg_file;
  Fl::args(argc, argv, arg_file, &arg_parser);
  // editor init
  win->label(title);
  buffer_init();
  if (arg_file < argc) {
    load_file(argv[arg_file]);
  }
  memset(search_string, 0, 2048);
  memset(replace_string, 0, 2048);
  win->callback((Fl_Callback *)close_cb, win);
  win->show(argc, argv);
  return Fl::run();
}