// smtp.h

#ifndef SMTP_H
#define SMTP_H

// Define the smtp_t structure (replace with your actual implementation)
typedef struct {
    // Add necessary fields for SMTP client
} smtp_t;

// Function prototypes
void smtp_init(smtp_t *smtp);
void smtp_set_server(smtp_t *smtp, const char *server, int port);
void smtp_set_auth(smtp_t *smtp, const char *email, const char *password);
void smtp_set_sender(smtp_t *smtp, const char *sender);
void smtp_add_recipient(smtp_t *smtp, const char *recipient);
void smtp_set_subject(smtp_t *smtp, const char *subject);
void smtp_set_body(smtp_t *smtp, const char *body);
int smtp_send(smtp_t *smtp);
void smtp_cleanup(smtp_t *smtp);

#endif // SMTP_H

