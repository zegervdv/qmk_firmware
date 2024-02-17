/*
Copyright 2024 @ jincao1, inspired by Keychron

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "rf_queue.h"

#define RPT_BUFFER_SIZE 64

static uint8_t queue_head = 0;
static uint8_t queue_tail = 0;

static report_buffer_t buffer_queue[RPT_BUFFER_SIZE] = {0};

bool enqueue_rf_report(report_buffer_t *report) {
    uint8_t next = (queue_head + 1) % RPT_BUFFER_SIZE;
    if (next == queue_tail) { // queue is full.
        return false;
    }
    buffer_queue[queue_head] = *report;
    queue_head               = next;
    return true;
}

bool dequeue_rf_report(report_buffer_t *report) {
    if (rf_queue_is_empty()) {
        return false; // queue empty
    }
    *report    = buffer_queue[queue_tail];
    queue_tail = (queue_tail + 1) % RPT_BUFFER_SIZE; // set tail to next index
    return true;
}

bool rf_queue_is_empty(void) {
    return queue_head == queue_tail;
}

void clear_rf_queue(void) {
    queue_head = 0;
    queue_tail = 0;
}