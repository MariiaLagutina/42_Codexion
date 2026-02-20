/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 19:26:31 by mlagutin          #+#    #+#             */
/*   Updated: 2026/02/05 19:41:28 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODERS_H
# define CODERS_H

# include <pthread.h>  // Для потоков и мьютексов
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <string.h>

/* 1. Общие правила симуляции */
typedef struct s_rules {
    int             nb_coders;
    int             time_burnout;
    int             time_compile;
    int             time_debug;
    int             time_refactor;
    int             nb_compiles;
    int             cooldown;
    int             is_edf;
    long long       start_time;
    int             stop_sim;
    pthread_mutex_t log_mutex;
    pthread_mutex_t stop_mutex;
    pthread_mutex_t *dongle_locks;
} t_rules;

/* 2. Данные конкретного кодера */
typedef struct s_coder {
    int             id;               //
    int             compiles_done;    // Сколько раз скомпилировал
    long long       last_meal;        // Время начала последней компиляции
    pthread_t       thread_id;        //
    pthread_mutex_t *left_dongle;     //
    pthread_mutex_t *right_dongle;    //
    t_rules         *rules;           // Ссылка на общие правила
} t_coder;

/* Прототипы функций (на будущее) */
long long   get_time(void);           //
void        print_status(t_coder *c, char *status); //

#endif