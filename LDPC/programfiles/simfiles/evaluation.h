void print_outputformat(errorcounts *);

void initialize_errorcount(int argc, char **argv, int l, errorcounts * errorcount,
                           simulations * simulation, randomizes *randomize,channels * channel, graphs * graph);
void count_error(graphs *, channels *, errorcounts *, simulations *);

int print_errors(int loop, int l, simulations *, errorcounts *, channels *, graphs *,
                 randomizes *);
void show_simulation_values(simulations *, errorcounts *);

void show_encoder_values(encoders *);

void show_channel_values(channels *, graphs *);

void show_random_values(randomizes *);

void show_error_vaoues(errorcounts *);

int counterrors(channels *);
