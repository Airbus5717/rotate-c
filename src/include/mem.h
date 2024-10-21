#pragma once

// Memory allocation
void *mem_alloc(usize size);
void *mem_resize(void *blk, usize size);
void mem_free(void *blk);
