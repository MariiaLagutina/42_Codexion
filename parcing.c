/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parcing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 18:44:37 by mlagutin          #+#    #+#             */
/*   Updated: 2026/02/05 19:44:02 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "coders.h"

int is_numeric(char *str) {
    if (!str || *str == '\0') return 0;
    for (int i = 0; str[i]; i++) {
        if (str[i] < '0' || str[i] > '9') return 0;
    }
    return 1;
}

int parse_args(t_rules *rules, int argc, char **argv) {
    if (argc != 9) return 0;
    for (int i = 1; i <= 7; i++) {
        if (!is_numeric(argv[i])) return 0;
    }
    rules->nb_coders = atoi(argv[1]);
    rules->time_burnout = atoi(argv[2]);
    rules->time_compile = atoi(argv[3]);
    rules->time_debug = atoi(argv[4]);
    rules->time_refactor = atoi(argv[5]);
    rules->nb_compiles = atoi(argv[6]);
    rules->cooldown = atoi(argv[7]);
    if (rules->nb_coders < 1) return 0;
    if (strcmp(argv[8], "edf") == 0)
        rules->is_edf = 1;
    else if (strcmp(argv[8], "fifo") == 0)
        rules->is_edf = 0;
    else
        return 0;

    return 1;
}

int main(int argc, char **argv)
{
    t_rules rules;

    if (!parse_args(&rules, argc, argv))
    {
        printf("Error: Invalid arguments.\n");
        return (1);
    }
    rules.start_time = get_time();
    printf("Arguments look good! Coders: %d, Mode: %s\n",
            rules.nb_coders, rules.is_edf ? "EDF" : "FIFO");
    return (0);
}
