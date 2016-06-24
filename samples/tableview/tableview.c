#include "libphone.h"

#define BACKGROUND_COLOR 0xefefef

static int backgroundView = 0;
static int tableView = 0;

static void appShowing(void) {
  phoneLog(PHONE_LOG_DEBUG, __FUNCTION__, "app showing");
}

static void appHiding(void) {
  phoneLog(PHONE_LOG_DEBUG, __FUNCTION__, "app hiding");
}

static void appTerminating(void) {
  phoneLog(PHONE_LOG_DEBUG, __FUNCTION__, "app terminating");
}

typedef struct cellContext {
  int iconView;
} cellContext;

static int onTableEvent(int handle, int eventType, void *param) {
  phoneLog(PHONE_LOG_DEBUG, __FUNCTION__, "table event:%s",
    phoneViewEventTypeToName(eventType));
  switch (eventType) {
    case PHONE_VIEW_REQUEST_TABLE_CELL_IDENTIFIER_TYPE_COUNT:
      return 1;
    case PHONE_VIEW_REQUEST_TABLE_CELL_IDENTIFIER: {
      phoneViewRequestTable *request = (phoneViewRequestTable *)param;
      phoneLog(PHONE_LOG_DEBUG, __FUNCTION__, "request section %d row %d identifier",
        request->section, request->row);
      return phoneCopyString(request->buf, request->bufSize, "testCell");
    } break;
    case PHONE_VIEW_REQUEST_TABLE_SECTION_COUNT:
      return 1;
    case PHONE_VIEW_REQUEST_TABLE_ROW_COUNT:
      return 10;
    case PHONE_VIEW_REQUEST_TABLE_ROW_HEIGHT:
      return dp(70);
    case PHONE_VIEW_REQUEST_TABLE_CELL_CUSTOM_VIEW: {
      phoneViewRequestTable *request = (phoneViewRequestTable *)param;
      int customView = phoneCreateContainerView(0, 0);
      cellContext *cell = (cellContext *)calloc(1, sizeof(cellContext));
      phoneLog(PHONE_LOG_DEBUG, __FUNCTION__,
          "custom view(handle:%d) created for section %d row %d",
          customView, request->section, request->row);
      phoneSetViewFrame(customView, 0, 0, phoneGetViewWidth(0),
        dp(70));
      phoneSetHandleTag(customView, cell);
      cell->iconView = phoneCreateContainerView(customView, 0);
      phoneSetViewFrame(cell->iconView, dp(10), dp(10), dp(50), dp(50));
      phoneSetViewCornerRadius(cell->iconView, dp(25));
      phoneSetViewBackgroundColor(cell->iconView, 0xff0000);
      phoneSetViewBackgroundImageResource(cell->iconView, "asset1.png");
      //phoneSetViewBorderColor(cell->iconView, 0x00ffff);
      //phoneSetViewBorderWidth(cell->iconView, dp(1));
      return customView;
    } break;
    case PHONE_VIEW_REQUEST_TABLE_CELL_RENDER: {
      phoneViewRequestTable *request = (phoneViewRequestTable *)param;
      phoneLog(PHONE_LOG_DEBUG, __FUNCTION__,
          "render view(handle:%d) for section %d row %d",
          request->renderHandle, request->section, request->row);
      if (request->renderHandle) {
        if (request->row % 2) {
          phoneSetViewBackgroundColor(request->renderHandle, 0xefefef);
        } else {
          phoneSetViewBackgroundColor(request->renderHandle, 0xcccccc);
        }
      }
    } break;
  }
  return PHONE_VIEW_EVENT_DONTCARE;
}

int phoneMain(int argc, const char *argv[]) {
  static phoneAppNotificationHandler handler = {
    appShowing,
    appHiding,
    appTerminating
  };
  phoneSetAppNotificationHandler(&handler);

  phoneLog(PHONE_LOG_DEBUG, __FUNCTION__, "display size: %f x %f",
    phoneGetViewWidth(0), phoneGetViewHeight(0));

  phoneSetStatusBarBackgroundColor(BACKGROUND_COLOR);
  backgroundView = phoneCreateContainerView(0, 0);
  phoneSetViewFrame(backgroundView, 0, 0, phoneGetViewWidth(0),
    phoneGetViewHeight(0));
  phoneSetViewBackgroundColor(backgroundView, BACKGROUND_COLOR);

  tableView = phoneCreateTableView(PHONE_TABLE_VIEW_STYLE_PLAIN,
    backgroundView, onTableEvent);
  phoneSetViewFrame(tableView, 0, 0, phoneGetViewWidth(0),
    phoneGetViewHeight(0));
  phoneReloadTableView(tableView);

  return 0;
}