#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

struct pre_state {
    const char *file;
    enum TYPE current_type;
};

/**
 * Pre-processes the given list.
 * @param list the list.
 * @param pre_state the current preprocessor state.
 */
void preprocessList(const struct token_list *list, struct pre_state *pre_state);

/**
 * Pre-processes the given list an initializes the preprocessor state,
 * static analysis and function definition is done over it.
 * @param list the list.
 * @param file the current file.
 */
void preprocess(const struct token_list *list, const char *file);

#endif /* PREPROCESSOR_H */
