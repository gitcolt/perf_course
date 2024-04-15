#ifndef JSON_GEN_H
#define JSON_GEN_H

void jg_init_context();
void jg_start();
void jg_end();
void jg_obj_start(char *key);
void jg_obj_end();
void jg_obj_key_val_int(char *key, int val);
void jg_obj_key_val_float(char *key, float val);
void jg_obj_key_val_str(char *key, char *val);
void jg_print();
void jg_write_to_file(const char *filename);
void jg_int(char *key, int val);
void jg_str(char *key, char *val);
void jg_arr_start(char *key);
void jg_arr_end();
void jg_arr_item_int(int val);
void jg_arr_item_float(float val);
void jg_arr_item_str(char *val);
void jg_destroy();

#endif
