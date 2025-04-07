#ifndef VCS_H
#define VCS_H

void init_vcfs();
void add_file(const std::string &filename);
void commit_changes(const std::string &message);
void show_log();
void add_all();

#endif