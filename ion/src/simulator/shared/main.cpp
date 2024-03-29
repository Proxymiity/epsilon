#include "haptics.h"
#include "journal.h"
#include "platform.h"
#include "random.h"
#include "state_file.h"
#include "telemetry.h"
#include "window.h"
#include <algorithm>
#include <assert.h>
#include <vector>
#include <ion.h>
#ifndef __WIN32__
#include <signal.h>
#include <sys/resource.h>
#endif
#if ION_SIMULATOR_FILES
#include "screenshot.h"
#include <signal.h>
#include "actions.h"
#endif

/* The Args class allows parsing and editing command-line arguments
 * The editing part allows us to add/remove arguments before forwarding them to
 * ion_main. */

class Args {
public:
  Args(int argc, char * argv[]) : m_arguments(argv, argv+argc) {}
  bool has(const char * key) const;
  const char * get(const char * key, bool pop = false);
  const char * pop(const char * key) { return get(key, true); }
  bool popFlag(const char * flag);
  void push(const char * key, const char * value) {
    if (key != nullptr && value != nullptr) {
      m_arguments.push_back(key);
      m_arguments.push_back(value);
    }
  }
  void push(const char * argument) { m_arguments.push_back(argument); }
  int argc() const { return m_arguments.size(); }
  const char * const * argv() const { return &m_arguments[0]; }
private:
  std::vector<const char *>::const_iterator find(const char * name) const;
  std::vector<const char *> m_arguments;
};

bool Args::has(const char * name) const {
  return find(name) != m_arguments.end();
}

const char * Args::get(const char * argument, bool pop) {
  auto nameIt = find(argument);
  if (nameIt != m_arguments.end()) {
    auto valueIt = std::next(nameIt);
    if (valueIt != m_arguments.end()) {
      const char * value = *valueIt;
      if (pop) {
        m_arguments.erase(valueIt);
        m_arguments.erase(nameIt);
      }
      return value;
    }
    if (pop) {
      m_arguments.erase(nameIt);
    }
  }
  return nullptr;
}

bool Args::popFlag(const char * argument) {
  auto flagIt = find(argument);
  if (flagIt != m_arguments.end()) {
    m_arguments.erase(flagIt);
    return true;
  }
  return false;
}

std::vector<const char *>::const_iterator Args::find(const char * name) const {
  return std::find_if(
    m_arguments.begin(), m_arguments.end(),
    [name](const char * s) { return strcmp(s, name) == 0; }
  );
}

using namespace Ion::Simulator;

int main(int argc, char * argv[]) {
  Args args(argc, argv);

#ifndef __WIN32__
  if (args.popFlag("--limit-stack-usage")) {
    // Limit stack usage
    constexpr int kStackSize = 32768;
    struct rlimit stackLimits = {kStackSize, kStackSize};
    setrlimit(RLIMIT_STACK, &stackLimits);
  }
#endif

#if ION_SIMULATOR_FILES
  const char * stateFile = args.pop("--load-state-file");
  if (stateFile) {
    assert(Journal::replayJournal());
    StateFile::load(stateFile);
    const char * replayJournalLanguage = Journal::replayJournal()->startingLanguage();
    if (replayJournalLanguage[0] != 0) {
      // Override any language setting if there is
      args.pop("--language");
      args.push("--language", replayJournalLanguage);
    }
  }

  const char * screenshotPath = args.pop("--take-screenshot");
  if (screenshotPath) {
    Ion::Simulator::Screenshot::commandlineScreenshot()->init(screenshotPath);
  }
#if !defined(_WIN32)
  signal(SIGUSR1, Ion::Simulator::Actions::handleUSR1Sig);
#endif
#endif

  // Default language
  if (!args.has("--language")) {
    args.push("--language", Platform::languageCode());
  }

  bool headless = args.popFlag("--headless");

  Random::init();
  if (!headless) {
    Journal::init();
    if (args.has("--language") && Journal::logJournal()) {
      // Set log journal starting language
      Journal::logJournal()->setStartingLanguage(args.get("--language"));
    }
#if EPSILON_TELEMETRY
    Telemetry::init();
#endif
    Window::init();
    Haptics::init();
  }
  ion_main(args.argc(), args.argv());
  if (!headless) {
    Haptics::shutdown();
    Window::shutdown();
#if EPSILON_TELEMETRY
    Telemetry::shutdown();
#endif
  }

  return 0;
}
