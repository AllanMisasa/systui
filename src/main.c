#include <ncurses.h>
#include <panel.h>

int main(int argc, char *argv[])
{
  initscr();
  noecho(); // Disable echoing of typed characters
  cbreak(); // Disable line buffering
  
  const char *aurmenu[] = {
    "Check packages",
    "Update packages",
    "Exit"
  };


  WINDOW *newpackages = newwin(15, 25, 2, 10);
  refresh();
  box(newpackages, 0, 0);
  mvwprintw(newpackages, 0, 1, "Available AUR updates");

  wrefresh(newpackages);
  getch();
  endwin();
  return 0;
};
