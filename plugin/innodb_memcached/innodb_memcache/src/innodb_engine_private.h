


void read_cmdline_options(struct innodb_engine *, struct default_engine *,
                          const char *);

int fetch_core_settings(struct innodb_engine *, struct default_engine *);

/*************** Declarations of functions that implement 
                 the engine interface 
 *********************************************************/
static const engine_info* innodb_get_info(ENGINE_HANDLE* handle);

static ENGINE_ERROR_CODE innodb_initialize(ENGINE_HANDLE* handle,
                                        const char* config_str);

static void innodb_destroy(ENGINE_HANDLE* handle, bool force);

static ENGINE_ERROR_CODE innodb_allocate(ENGINE_HANDLE* handle,
                                      const void* cookie,
                                      item **item,
                                      const void* key,
                                      const size_t nkey,
                                      const size_t nbytes,
                                      const int flags,
                                      const rel_time_t exptime);

static ENGINE_ERROR_CODE innodb_remove(ENGINE_HANDLE* handle,
                                    const void* cookie,
                                    const void* key,
                                    const size_t nkey,
                                    uint64_t cas,
                                    uint16_t vbucket);

static void innodb_release(ENGINE_HANDLE* handle,
                        const void *cookie,
                        item* item);

static ENGINE_ERROR_CODE innodb_get(ENGINE_HANDLE* handle,
                                 const void* cookie,
                                 item** item,
                                 const void* key,
                                 const int nkey,
                                 uint16_t vbucket);

static ENGINE_ERROR_CODE innodb_get_stats(ENGINE_HANDLE* handle,
                                       const void *cookie,
                                       const char *stat_key,
                                       int nkey,
                                       ADD_STAT add_stat);

static void innodb_reset_stats(ENGINE_HANDLE* handle, 
                            const void *cookie);

static ENGINE_ERROR_CODE innodb_store(ENGINE_HANDLE* handle,
                                   const void *cookie,
                                   item* item,
                                   uint64_t *cas,
                                   ENGINE_STORE_OPERATION operation,
                                   uint16_t vbucket);

static ENGINE_ERROR_CODE innodb_arithmetic(ENGINE_HANDLE* handle,
                                        const void* cookie,
                                        const void* key,
                                        const int nkey,
                                        const bool increment,
                                        const bool create,
                                        const uint64_t delta,
                                        const uint64_t initial,
                                        const rel_time_t exptime,
                                        uint64_t *cas,
                                        uint64_t *result,
                                        uint16_t vbucket);

static ENGINE_ERROR_CODE innodb_flush(ENGINE_HANDLE* handle,
                                   const void* cookie, 
                                   time_t when);

static ENGINE_ERROR_CODE innodb_unknown_command(ENGINE_HANDLE* handle,
                                             const void* cookie,
                                             protocol_binary_request_header *request,
                                             ADD_RESPONSE response);

static bool innodb_get_item_info(ENGINE_HANDLE *handle, 
                              const void *cookie,
                              const item* item, 
                              item_info *item_info);
