# GitHub Copilot Code Review Instructions

This file guides GitHub Copilot's automated reviews for this open-source project. Maintainers can modify these criteria to enforce repo-specific coding standards.

## 🎯 High-Priority Focus Areas

### 1. Architectural Alignment
* Ensure new code aligns with the repository's existing pattern.
* Do not duplicate built-in utility modules.
* Flag tightly coupled modules or missing abstraction layers.

### 2. Performance & Efficiency
* Check for redundant database operations, unindexed lookups, or nested loops.
* Ensure large datasets are streamed or paginated rather than held entirely in memory.

### 3. Security Essentials
* Intercept raw SQL inputs or vulnerable string concatenations.
* Verify proper authorization or permission logic on newly exposed api routes.
* Ensure zero hardcoded keys or secrets reach the commits.

### 4. Code Quality & Formatting
* Request detailed variable and method descriptors instead of ambiguous abbreviations.
* Block overly complex functions exceeding 20 lines without separate subroutines.

---

## 🚦 Review Behavior Preferences

* **Tone:** Maintain a peer-like, objective, and constructive feedback style.
* **Format:** Provide code suggestions natively formatted as GitHub codeblocks (`diff`) whenever applicable for easy application.
* **Handling False Positives:** If a practice seems unconventional but is explicitly documented in the local comments, pass it without a flag.