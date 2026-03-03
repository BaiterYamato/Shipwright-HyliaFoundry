# Reconcile Policy

Rules:
1. `Plans.md` status is authoritative for plan lifecycle.
2. Memory corrections must be append-only (`supersedes` when applicable).
3. Report first; apply fixes only when explicitly requested.

Checks:
- done plans with open `plan:<id>` memory topic
- memory `plan:*` topics with no ledger entry
- duplicate plan task topics still open after completion
