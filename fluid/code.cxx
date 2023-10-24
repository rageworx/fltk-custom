//
// Code output routines for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2023 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     https://www.fltk.org/COPYING.php
//
// Please see the following page on how to report bugs and issues:
//
//     https://www.fltk.org/bugs.php
//

#include "code.h"

#include "Fl_Group_Type.h"
#include "Fl_Window_Type.h"
#include "Fl_Function_Type.h"
#include "alignment_panel.h"
#include "file.h"

#include <FL/Fl.H>
#include <FL/fl_string_functions.h>
#include "fluid_filename.h"
#include "../src/flstring.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/// \defgroup cfile C Code File Operations
/// \{


/**
 Return true if c can be in a C identifier.
 I needed this so it is not messed up by locale settings.
 */
int is_id(char c) {
  return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_';
}

/**
 Write a file that contains all label and tooltip strings for internationalisation.
 */
int write_strings(const Fl_String &filename) {
  Fl_Type *p;
  Fl_Widget_Type *w;
  int i;

  FILE *fp = fl_fopen(filename.c_str(), "w");
  if (!fp) return 1;

  switch (g_project.i18n_type) {
    case 0 : /* None, just put static text out */
      fprintf(fp, "# generated by Fast Light User Interface Designer (fluid) version %.4f\n",
              FL_VERSION);
      for (p = Fl_Type::first; p; p = p->next) {
        if (p->is_widget()) {
          w = (Fl_Widget_Type *)p;

          if (w->label()) {
            for (const char *s = w->label(); *s; s ++)
              if (*s < 32 || *s > 126 || *s == '\"')
                fprintf(fp, "\\%03o", *s);
              else
                putc(*s, fp);
            putc('\n', fp);
          }

          if (w->tooltip()) {
            for (const char *s = w->tooltip(); *s; s ++)
              if (*s < 32 || *s > 126 || *s == '\"')
                fprintf(fp, "\\%03o", *s);
              else
                putc(*s, fp);
            putc('\n', fp);
          }
        }
      }
      break;
    case 1 : /* GNU gettext, put a .po file out */
      fprintf(fp, "# generated by Fast Light User Interface Designer (fluid) version %.4f\n",
              FL_VERSION);
      for (p = Fl_Type::first; p; p = p->next) {
        if (p->is_widget()) {
          w = (Fl_Widget_Type *)p;

          if (w->label()) {
            const char *s;

            fputs("msgid \"", fp);
            for (s = w->label(); *s; s ++)
              if (*s < 32 || *s > 126 || *s == '\"')
                fprintf(fp, "\\%03o", *s);
              else
                putc(*s, fp);
            fputs("\"\n", fp);

            fputs("msgstr \"", fp);
            for (s = w->label(); *s; s ++)
              if (*s < 32 || *s > 126 || *s == '\"')
                fprintf(fp, "\\%03o", *s);
              else
                putc(*s, fp);
            fputs("\"\n", fp);
          }

          if (w->tooltip()) {
            const char *s;

            fputs("msgid \"", fp);
            for (s = w->tooltip(); *s; s ++)
              if (*s < 32 || *s > 126 || *s == '\"')
                fprintf(fp, "\\%03o", *s);
              else
                putc(*s, fp);
            fputs("\"\n", fp);

            fputs("msgstr \"", fp);
            for (s = w->tooltip(); *s; s ++)
              if (*s < 32 || *s > 126 || *s == '\"')
                fprintf(fp, "\\%03o", *s);
              else
                putc(*s, fp);
            fputs("\"\n", fp);
          }
        }
      }
      break;
    case 2 : /* POSIX catgets, put a .msg file out */
      fprintf(fp, "$ generated by Fast Light User Interface Designer (fluid) version %.4f\n",
              FL_VERSION);
      fprintf(fp, "$set %s\n", g_project.i18n_pos_set.c_str());
      fputs("$quote \"\n", fp);

      for (i = 1, p = Fl_Type::first; p; p = p->next) {
        if (p->is_widget()) {
          w = (Fl_Widget_Type *)p;

          if (w->label()) {
            fprintf(fp, "%d \"", i ++);
            for (const char *s = w->label(); *s; s ++)
              if (*s < 32 || *s > 126 || *s == '\"')
                fprintf(fp, "\\%03o", *s);
              else
                putc(*s, fp);
            fputs("\"\n", fp);
          }

          if (w->tooltip()) {
            fprintf(fp, "%d \"", i ++);
            for (const char *s = w->tooltip(); *s; s ++)
              if (*s < 32 || *s > 126 || *s == '\"')
                fprintf(fp, "\\%03o", *s);
              else
                putc(*s, fp);
            fputs("\"\n", fp);
          }
        }
      }
      break;
  }

  return fclose(fp);
}

////////////////////////////////////////////////////////////////
// Generate unique but human-readable identifiers:

struct Fd_Identifier_Tree {
  char* text;
  void* object;
  Fd_Identifier_Tree *left, *right;
  Fd_Identifier_Tree (const char* t, void* o) : text(fl_strdup(t)), object(o) {left = right = 0;}
  ~Fd_Identifier_Tree();
};

Fd_Identifier_Tree::~Fd_Identifier_Tree() {
  delete left;
  free((void *)text);
  delete right;
}

/** \brief Return a unique name for the given object.

 This function combines the name and label into an identifier. It then checks
 if that id was already taken by another object, and if so, appends a
 hexadecimal value which is incremented until the id is unique in this file.

 If a new id was created, it is stored in the id tree.

 \param[in] o create an ID for this object
 \param[in] type is the first word of the ID
 \param[in] name if name is set, it is appended to the ID
 \param[in] label else if label is set, it is appended, skipping non-keyword characters
 \return buffer to a unique identifier, managed by Fd_Code_Writer, so caller must NOT free() it
 */
const char* Fd_Code_Writer::unique_id(void* o, const char* type, const char* name, const char* label) {
  char buffer[128];
  char* q = buffer;
  char* q_end = q + 128 - 8 - 1; // room for hex number and NUL
  while (*type) *q++ = *type++;
  *q++ = '_';
  const char* n = name;
  if (!n || !*n) n = label;
  if (n && *n) {
    while (*n && !is_id(*n)) n++;
    while (is_id(*n) && (q < q_end)) *q++ = *n++;
  }
  *q = 0;
  // okay, search the tree and see if the name was already used:
  Fd_Identifier_Tree** p = &id_root;
  int which = 0;
  while (*p) {
    int i = strcmp(buffer, (*p)->text);
    if (!i) {
      if ((*p)->object == o) return (*p)->text;
      // already used, we need to pick a new name:
      sprintf(q,"%x",++which);
      p = &id_root;
      continue;
    }
    else if (i < 0) p = &((*p)->left);
    else p  = &((*p)->right);
  }
  *p = new Fd_Identifier_Tree(buffer, o);
  return (*p)->text;
}

////////////////////////////////////////////////////////////////
// return current indentation:


/**
 Return a C string that indents code to the given depth.

 Indentation can be changed by modifying the multiplicator (``*2`` to keep
 the FLTK indent style). Changing `spaces` to a list of tabs would generate
 tab indents instead. This function can also be used for fixed depth indents
 in the header file.

 Do *not* ever make this a user preference, or you will end up writing a
 fully featured code formatter.

 \param[in] set generate this indent depth
 \return pointer to a static string
 */
const char *Fd_Code_Writer::indent(int set) {
  static const char* spaces = "                                ";
  int i = set * 2;
  if (i>32) i = 32;
  if (i<0) i = 0;
  return spaces+32-i;
}

/**
 Return a C string that indents code to the current source file depth.
 \return pointer to a static string
 */
const char *Fd_Code_Writer::indent() {
  return indent(indentation);
}

/**
 Return a C string that indents code to the current source file depth plus an offset.
 \param[in] offset adds a temporary offset for this call only; this does not
    change the `indentation` variable; offset can be negative
 \return pointer to a static string
 */
const char *Fd_Code_Writer::indent_plus(int offset) {
  return indent(indentation+offset);
}


////////////////////////////////////////////////////////////////
// declarations/include files:
// Each string generated by write_h_once is written only once to
// the header file.  This is done by keeping a binary tree of all
// the calls so far and not printing it if it is in the tree.

struct Fd_Text_Tree {
  char *text;
  Fd_Text_Tree *left, *right;
  Fd_Text_Tree(const char *t) {
    text = fl_strdup(t);
    left = right = 0;
  }
  ~Fd_Text_Tree();
};

Fd_Text_Tree::~Fd_Text_Tree() {
  delete left;
  free((void *)text);
  delete right;
}

struct Fd_Pointer_Tree {
  void *ptr;
  Fd_Pointer_Tree *left, *right;
  Fd_Pointer_Tree(void *p) {
    ptr = p;
    left = right = 0;
  }
  ~Fd_Pointer_Tree();
};

Fd_Pointer_Tree::~Fd_Pointer_Tree() {
  delete left;
  delete right;
}

/**
 Print a formatted line to the header file, unless the same line was produced before in this header file.
 \param[in] format printf-style formatting text, followed by a vararg list
 */
int Fd_Code_Writer::write_h_once(const char *format, ...) {
  va_list args;
  char buf[1024];
  va_start(args, format);
  vsnprintf(buf, sizeof(buf), format, args);
  va_end(args);
  Fd_Text_Tree **p = &text_in_header;
  while (*p) {
    int i = strcmp(buf,(*p)->text);
    if (!i) return 0;
    else if (i < 0) p = &((*p)->left);
    else p  = &((*p)->right);
  }
  fprintf(header_file,"%s\n",buf);
  *p = new Fd_Text_Tree(buf);
  return 1;
}

/**
 Print a formatted line to the source file, unless the same line was produced before in this code file.
 \param[in] format printf-style formatting text, followed by a vararg list
 */
int Fd_Code_Writer::write_c_once(const char *format, ...) {
  va_list args;
  char buf[1024];
  va_start(args, format);
  vsnprintf(buf, sizeof(buf), format, args);
  va_end(args);
  Fd_Text_Tree **p = &text_in_header;
  while (*p) {
    int i = strcmp(buf,(*p)->text);
    if (!i) return 0;
    else if (i < 0) p = &((*p)->left);
    else p  = &((*p)->right);
  }
  p = &text_in_code;
  while (*p) {
    int i = strcmp(buf,(*p)->text);
    if (!i) return 0;
    else if (i < 0) p = &((*p)->left);
    else p  = &((*p)->right);
  }
  fprintf(code_file,"%s\n",buf);
  *p = new Fd_Text_Tree(buf);
  return 1;
}

/**
 Return true if this pointer was already included in the code file.
 If it was not, add it to the list and return false.
 */
bool Fd_Code_Writer::c_contains(void *pp) {
  Fd_Pointer_Tree **p = &ptr_in_code;
  while (*p) {
    if ((*p)->ptr == pp) return true;
    else if ((*p)->ptr < pp) p = &((*p)->left);
    else p  = &((*p)->right);
  }
  *p = new Fd_Pointer_Tree(pp);
  return false;
}

/**
 Write a C string to the code file, escaping non-ASCII characters.

 Adds " before and after the text.

 A list of control characters and ", ', and \\ are escaped by adding a \\ in
 front of them. Escape ?? by writing ?\\?. All other characters that are not
 between 32 and 126 inclusive will be escaped as octal characters.

 This function is utf8 agnostic.

 \param[in] s write this string
 \param[in] length write so many bytes in this string

 \see f.write_cstring(const char*)
 */
void Fd_Code_Writer::write_cstring(const char *s, int length) {
  if (varused_test) {
    varused = 1;
    return;
  }
  // if we are rendering to the source code preview window, and the text is
  // longer than four lines, we only render a placeholder.
  if (write_sourceview && ((s==NULL) || (length>300))) {
    if (length>=0)
      fprintf(code_file, "\" ... %d bytes of text... \"", length);
    else
      fprintf(code_file, "\" ... text... \"");
    return;
  }
  if (length==-1 || s==0L) {
    fprintf(code_file, "\n#error  string not found\n");
    fprintf(code_file, "\" ... undefined size text... \"");
    return;
  }

  const char *p = s;
  const char *e = s+length;
  int linelength = 1;
  putc('\"', code_file);
  for (; p < e;) {
    int c = *p++;
    switch (c) {
    case '\b': c = 'b'; goto QUOTED;
    case '\t': c = 't'; goto QUOTED;
    case '\n': c = 'n'; goto QUOTED;
    case '\f': c = 'f'; goto QUOTED;
    case '\r': c = 'r'; goto QUOTED;
    case '\"':
    case '\'':
    case '\\':
    QUOTED:
      if (linelength >= 77) {fputs("\\\n",code_file); linelength = 0;}
      putc('\\', code_file);
      putc(c, code_file);
      linelength += 2;
      break;
    case '?': // prevent trigraphs by writing ?? as ?\?
      if (p-2 >= s && *(p-2) == '?') goto QUOTED;
      // else fall through:
    default:
      if (c >= ' ' && c < 127) {
        // a legal ASCII character
        if (linelength >= 78) {fputs("\\\n",code_file); linelength = 0;}
        putc(c, code_file);
        linelength++;
        break;
      }
      // if the UTF-8 option is checked, write unicode characters verbatim
        if (g_project.utf8_in_src && (c&0x80)) {
          if ((c&0x40)) {
            // This is the first character in a utf-8 sequence (0b11......).
            // A line break would be ok here. Do not put linebreak in front of
            // following characters (0b10......)
            if (linelength >= 78) {fputs("\\\n",code_file); linelength = 0;}
          }
          putc(c, code_file);
          linelength++;
          break;
        }
      // otherwise we must print it as an octal constant:
      c &= 255;
      if (c < 8) {
        if (linelength >= 76) {fputs("\\\n",code_file); linelength = 0;}
        fprintf(code_file, "\\%o",c);
        linelength += 2;
      } else if (c < 64) {
        if (linelength >= 75) {fputs("\\\n",code_file); linelength = 0;}
        fprintf(code_file, "\\%o",c);
        linelength += 3;
      } else {
        if (linelength >= 74) {fputs("\\\n",code_file); linelength = 0;}
        fprintf(code_file, "\\%o",c);
        linelength += 4;
      }
      // We must not put more numbers after it, because some C compilers
      // consume them as part of the quoted sequence.  Use string constant
      // pasting to avoid this:
      c = *p;
      if (p < e && ( (c>='0'&&c<='9') || (c>='a'&&c<='f') || (c>='A'&&c<='F') )) {
        putc('\"', code_file); linelength++;
        if (linelength >= 79) {fputs("\n",code_file); linelength = 0;}
        putc('\"', code_file); linelength++;
      }
      break;
    }
  }
  putc('\"', code_file);
}

/**
 Write a C string, escaping non-ASCII characters.
 \param[in] s write this string
 \see f.write_cstring(const char*, int)
 */
void Fd_Code_Writer::write_cstring(const char *s) {
  write_cstring(s, (int)strlen(s));
}

/**
 Write an array of C binary data (does not add a null).
 The output is bracketed in { and }. The content is written
 as decimal bytes, i.e. `{ 1, 2, 200 }`
 */
void Fd_Code_Writer::write_cdata(const char *s, int length) {
  if (varused_test) {
    varused = 1;
    return;
  }
  if (write_sourceview) {
    if (length>=0)
      fprintf(code_file, "{ /* ... %d bytes of binary data... */ }", length);
    else
      fprintf(code_file, "{ /* ... binary data... */ }");
    return;
  }
  if (length==-1) {
    fprintf(code_file, "\n#error  data not found\n");
    fprintf(code_file, "{ /* ... undefined size binary data... */ }");
    return;
  }
  const unsigned char *w = (const unsigned char *)s;
  const unsigned char *e = w+length;
  int linelength = 1;
  putc('{', code_file);
  for (; w < e;) {
    unsigned char c = *w++;
    if (c>99) linelength += 4;
    else if (c>9) linelength += 3;
    else linelength += 2;
    if (linelength >= 77) {fputs("\n",code_file); linelength = 0;}
    fprintf(code_file, "%d", c);
    if (w<e) putc(',', code_file);
  }
  putc('}', code_file);
}

/**
 Print a formatted line to the source file.
 \param[in] format printf-style formatting text
 \param[in] args list of arguments
 */
void Fd_Code_Writer::vwrite_c(const char* format, va_list args) {
  if (varused_test) {
    varused = 1;
    return;
  }
  vfprintf(code_file, format, args);
}

/**
 Print a formatted line to the source file.
 \param[in] format printf-style formatting text, followed by a vararg list
 */
void Fd_Code_Writer::write_c(const char* format,...) {
  va_list args;
  va_start(args, format);
  vwrite_c(format, args);
  va_end(args);
}

/**
 Write code (c) of size (n) to C file, with optional comment (com) w/o trailing space.
 if the code line does not end in a ';' or '}', a ';' will be added.
 \param[in] indent indentation string for all lines
 \param[in] n number of bytes in code line
 \param[in] c line of code
 \param[in] com optional commentary
 */
void Fd_Code_Writer::write_cc(const char *indent, int n, const char *c, const char *com) {
  write_c("%s%.*s", indent, n, c);
  char cc = c[n-1];
  if (cc!='}' && cc!=';')
    write_c(";");
  if (*com)
    write_c(" %s", com);
  write_c("\n");
}

/**
 Print a formatted line to the header file.
 \param[in] format printf-style formatting text, followed by a vararg list
 */
void Fd_Code_Writer::write_h(const char* format,...) {
  if (varused_test) return;
  va_list args;
  va_start(args, format);
  vfprintf(header_file, format, args);
  va_end(args);
}

/**
 Write code (c) of size (n) to H file, with optional comment (com) w/o trailing space.
 if the code line does not end in a ';' or '}', a ';' will be added.
 \param[in] indent indentation string for all lines
 \param[in] n number of bytes in code line
 \param[in] c line of code
 \param[in] com optional commentary
 */
void Fd_Code_Writer::write_hc(const char *indent, int n, const char* c, const char *com) {
  write_h("%s%.*s", indent, n, c);
  char cc = c[n-1];
  if (cc!='}' && cc!=';')
    write_h(";");
  if (*com)
    write_h(" %s", com);
  write_h("\n");
}

/**
 Write one or more lines of code, indenting each one of them.
 \param[in] textlines one or more lines of text, separated by \\n
 */
void Fd_Code_Writer::write_c_indented(const char *textlines, int inIndent, char inTrailwWith) {
  if (textlines) {
    indentation += inIndent;
    for (;;) {
      int line_len;
      const char *newline = strchr(textlines, '\n');
      if (newline)
        line_len = (int)(newline-textlines);
      else
        line_len = (int)strlen(textlines);
      if (textlines[0]=='\n') {
        // avoid trailing spaces
      } else if (textlines[0]=='#') {
        // don't indent preprocessor statments starting with '#'
        write_c("%.*s", line_len, textlines);
      } else {
        // indent all other text lines
        write_c("%s%.*s", indent(), line_len, textlines);
      }
      if (newline) {
        write_c("\n");
      } else {
        if (inTrailwWith)
          write_c("%c", inTrailwWith);
        break;
      }
      textlines = newline+1;
    }
    indentation -= inIndent;
  }
}


/**
 Recursively dump code, putting children between the two parts of the parent code.
 */
Fl_Type* Fd_Code_Writer::write_code(Fl_Type* p) {
  if (write_sourceview) {
    p->code_position = (int)ftell(code_file);
    if (p->header_position_end==-1)
      p->header_position = (int)ftell(header_file);
  }
  // write all code that come before the children code
  // (but don't write the last comment until the very end)
  if (!(p==Fl_Type::last && p->is_a(ID_Comment)))
    p->write_code1(*this);
  // recursively write the code of all children
  Fl_Type* q;
  if (p->is_widget() && p->is_class()) {
    // Handle widget classes specially
    for (q = p->next; q && q->level > p->level;) {
      if (!q->is_a(ID_Function)) q = write_code(q);
      else {
        int level = q->level;
        do {
          q = q->next;
        } while (q && q->level > level);
      }
    }

    // write all code that come after the children
    p->write_code2(*this);

    for (q = p->next; q && q->level > p->level;) {
      if (q->is_a(ID_Function)) q = write_code(q);
      else {
        int level = q->level;
        do {
          q = q->next;
        } while (q && q->level > level);
      }
    }

    write_h("};\n");
    current_widget_class = 0L;
  } else {
    for (q = p->next; q && q->level > p->level;) q = write_code(q);
    // write all code that come after the children
    p->write_code2(*this);
  }
  if (write_sourceview) {
    p->code_position_end = (int)ftell(code_file);
    if (p->header_position_end==-1)
      p->header_position_end = (int)ftell(header_file);
  }
  return q;
}

/**
 Write the source and header files for the current design.

 If the files already exist, they will be overwritten.

 \param[in] s filename of source code file
 \param[in] t filename of the header file
 \return 0 if the operation failed, 1 if it was successful
 */
int Fd_Code_Writer::write_code(const char *s, const char *t, bool to_sourceview) {
  write_sourceview = to_sourceview;
  const char *filemode = "w";
  if (write_sourceview)
    filemode = "wb";
  delete id_root; id_root = 0;
  indentation = 0;
  current_class = 0L;
  current_widget_class = 0L;
  if (!s) code_file = stdout;
  else {
    FILE *f = fl_fopen(s, filemode);
    if (!f) return 0;
    code_file = f;
  }
  if (!t) header_file = stdout;
  else {
    FILE *f = fl_fopen(t, filemode);
    if (!f) {fclose(code_file); return 0;}
    header_file = f;
  }
  // if the first entry in the Type tree is a comment, then it is probably
  // a copyright notice. We print that before anything else in the file!
  Fl_Type* first_type = Fl_Type::first;
  if (first_type && first_type->is_a(ID_Comment)) {
    if (write_sourceview) {
      first_type->code_position = (int)ftell(code_file);
      first_type->header_position = (int)ftell(header_file);
    }
    // it is ok to write non-recursive code here, because comments have no children or code2 blocks
    first_type->write_code1(*this);
    if (write_sourceview) {
      first_type->code_position_end = (int)ftell(code_file);
      first_type->header_position_end = (int)ftell(header_file);
    }
    first_type = first_type->next;
  }

  const char *hdr = "\
// generated by Fast Light User Interface Designer (fluid) version %.4f\n\n";
  fprintf(header_file, hdr, FL_VERSION);
  fprintf(code_file, hdr, FL_VERSION);

  {char define_name[102];
  const char* a = fl_filename_name(t);
  char* b = define_name;
  if (!isalpha(*a)) {*b++ = '_';}
  while (*a) {*b++ = isalnum(*a) ? *a : '_'; a++;}
  *b = 0;
  fprintf(header_file, "#ifndef %s\n", define_name);
  fprintf(header_file, "#define %s\n", define_name);
  }

  if (g_project.avoid_early_includes==0) {
    write_h_once("#include <FL/Fl.H>");
  }
  if (t && g_project.include_H_from_C) {
    if (to_sourceview) {
      write_c("#include \"CodeView.h\"\n");
    } else if (g_project.header_file_name[0] == '.' && strchr(g_project.header_file_name.c_str(), '/') == NULL) {
      write_c("#include \"%s\"\n", fl_filename_name(t));
    } else {
      write_c("#include \"%s\"\n", g_project.header_file_name.c_str());
    }
  }
  Fl_String loc_include, loc_conditional;
  if (g_project.i18n_type==1) {
    loc_include = g_project.i18n_gnu_include;
    loc_conditional = g_project.i18n_gnu_conditional;
  } else {
    loc_include = g_project.i18n_pos_include;
    loc_conditional = g_project.i18n_pos_conditional;
  }
  if (g_project.i18n_type && !loc_include.empty()) {
    int conditional = !loc_conditional.empty();
    if (conditional) {
      write_c("#ifdef %s\n", loc_conditional.c_str());
      indentation++;
    }
    if (loc_include[0] != '<' && loc_include[0] != '\"')
      write_c("#%sinclude \"%s\"\n", indent(), loc_include.c_str());
    else
      write_c("#%sinclude %s\n", indent(), loc_include.c_str());
    if (g_project.i18n_type == 2) {
      if (!g_project.i18n_pos_file.empty()) {
        write_c("extern nl_catd %s;\n", g_project.i18n_pos_file.c_str());
      } else {
        write_c("// Initialize I18N stuff now for menus...\n");
        write_c("#%sinclude <locale.h>\n", indent());
        write_c("static char *_locale = setlocale(LC_MESSAGES, \"\");\n");
        write_c("static nl_catd _catalog = catopen(\"%s\", 0);\n", g_project.basename().c_str());
      }
    }
    if (conditional) {
      write_c("#else\n");
      if (g_project.i18n_type == 1) {
        if (!g_project.i18n_gnu_function.empty()) {
          write_c("#%sifndef %s\n", indent(), g_project.i18n_gnu_function.c_str());
          write_c("#%sdefine %s(text) text\n", indent_plus(1), g_project.i18n_gnu_function.c_str());
          write_c("#%sendif\n", indent());
        }
      }
      if (g_project.i18n_type == 2) {
        write_c("#%sifndef catgets\n", indent());
        write_c("#%sdefine catgets(catalog, set, msgid, text) text\n", indent_plus(1));
        write_c("#%sendif\n", indent());
      }
      indentation--;
      write_c("#endif\n");
    }
    if (g_project.i18n_type == 1 && g_project.i18n_gnu_static_function[0]) {
      write_c("#ifndef %s\n", g_project.i18n_gnu_static_function.c_str());
      write_c("#%sdefine %s(text) text\n", indent_plus(1), g_project.i18n_gnu_static_function.c_str());
      write_c("#endif\n");
    }
  }
  for (Fl_Type* p = first_type; p;) {
    // write all static data for this & all children first
    if (write_sourceview) p->header_position = (int)ftell(header_file);
    p->write_static(*this);
    if (write_sourceview) {
      p->header_position_end = (int)ftell(header_file);
      if (p->header_position==p->header_position_end) p->header_position_end = -1;
    }
    for (Fl_Type* q = p->next; q && q->level > p->level; q = q->next) {
      if (write_sourceview) q->header_position = (int)ftell(header_file);
      q->write_static(*this);
      if (write_sourceview) {
        q->header_position_end = (int)ftell(header_file);
        if (q->header_position==q->header_position_end) q->header_position_end = -1;
      }
    }
    // then write the nested code:
    p = write_code(p);
  }

  if (!s) return 1;

  fprintf(header_file, "#endif\n");

  Fl_Type* last_type = Fl_Type::last;
  if (last_type && last_type->is_a(ID_Comment)) {
    if (write_sourceview) {
      last_type->code_position = (int)ftell(code_file);
      last_type->header_position = (int)ftell(header_file);
    }
    last_type->write_code1(*this);
    if (write_sourceview) {
      last_type->code_position_end = (int)ftell(code_file);
      last_type->header_position_end = (int)ftell(header_file);
    }
  }
  int x = 0, y = 0;

  if (code_file != stdout)
    x = fclose(code_file);
  code_file = 0;
  if (header_file != stdout)
    y = fclose(header_file);
  header_file = 0;
  return x >= 0 && y >= 0;
}


/**
 Write the public/private/protected keywords inside the class.
 This avoids repeating these words if the mode is already set.
 */
void Fd_Code_Writer::write_public(int state) {
  if (!current_class && !current_widget_class) return;
  if (current_class && current_class->write_public_state == state) return;
  if (current_widget_class && current_widget_class->write_public_state == state) return;
  if (current_class) current_class->write_public_state = state;
  if (current_widget_class) current_widget_class->write_public_state = state;
  switch (state) {
    case 0: write_h("private:\n"); break;
    case 1: write_h("public:\n"); break;
    case 2: write_h("protected:\n"); break;
  }
}


Fd_Code_Writer::Fd_Code_Writer()
: code_file(NULL),
  header_file(NULL),
  id_root(NULL),
  text_in_header(NULL),
  text_in_code(NULL),
  ptr_in_code(NULL),
  indentation(0),
  write_sourceview(false),
  varused_test(0),
  varused(0)
{
}

Fd_Code_Writer::~Fd_Code_Writer()
{
  delete id_root;
  delete ptr_in_code;
  delete text_in_code;
  delete text_in_header;
}

/// \}

