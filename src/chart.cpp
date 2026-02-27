#include "chart.h"

#include <atomic>
#include <chrono>
#include <time.h>
#include <memory>


namespace {

struct ReferenceCount {
  std::atomic<int> _ref_count;
  ReferenceCount() :
    _ref_count(1) {}
};

struct ChartSeriesContext {
  std::string _series_name;
  lv_chart_series_t *_lv_series = nullptr;
  size_t _num_points = 0;
  lv_coord_t *_data = nullptr;
  ReferenceCount *_rc = nullptr;

  ChartSeriesContext(
    lv_obj_t *chart,
    int num_xlabels,
    const std::string &series_name);
  ChartSeriesContext(const ChartSeriesContext& rhs);
  const ChartSeriesContext &operator=(const ChartSeriesContext& rhs) = delete;
  ~ChartSeriesContext();
};

struct ChartContext {
  std::string _type;
  std::vector<std::string> _xlabels;
  std::vector<std::string> _ylabels;
  std::string _update_timer_function;
  std::vector<ChartSeriesContext> _series_contexts;
  int _num_series = 0;
  lv_timer_t *_update_timer = nullptr;

  ChartContext(
    const std::string& id, 
    const std::vector<std::string> &xlabels, 
    const std::vector<std::string> &ylabels, 
    const std::string &update_timer_function,
    const std::vector<ChartSeriesContext> &series_contexts,
    lv_obj_t *chart,
    int num_series, 
    int update_hz
  );

  static void 
  chartUpdateTimer(struct _lv_timer_t *timer);
};

// static void 
// onDrawChart(lv_event_t *e) {

//   lv_obj_t *chart = (lv_obj_t *)e->user_data;
//   lv_obj_draw_part_dsc_t * dsc = (lv_obj_draw_part_dsc_t*)lv_event_get_param(e);
//   if(dsc->part == LV_PART_TICKS && dsc->id == LV_CHART_AXIS_PRIMARY_X) {
//     const struct ChartContext *ctx = (const struct ChartContext *)chart->user_data;
//     lv_snprintf(dsc->text, sizeof(dsc->text), "%s", ctx->_xlabels[dsc->value].c_str());
//   }
// }

} //namespace

ChartSeriesContext::ChartSeriesContext(
    lv_obj_t *chart,
    int num_xlabels,
    const std::string &series_name) 
  : _series_name(series_name)
  , _lv_series(lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y))
  , _num_points(num_xlabels)
  // , _data(std::make_shared<lv_coord_t[]>(num_xlabels)) // does not work with arm builds, using aarch64-oe-linux-g++ (GCC) 11.3.0
  , _data(static_cast<lv_coord_t*>(calloc(sizeof(lv_coord_t), num_xlabels)))
  , _rc(new ReferenceCount)  
{}

ChartSeriesContext::ChartSeriesContext(const ChartSeriesContext& rhs)
  : _series_name(rhs._series_name)
  , _lv_series(rhs._lv_series)
  , _num_points(rhs._num_points)
  , _data(rhs._data)
  , _rc(rhs._rc)   {

  _rc->_ref_count++;
}

ChartSeriesContext::~ChartSeriesContext(){

  --_rc->_ref_count;
  if(_rc->_ref_count == 0) {
    free(_data);
    delete _rc;
  }
}

static void 
updateChart1(
  lv_obj_t *chart)
{
    if(!chart) { return; }

    srand(time(0));
 
    ChartContext *ctx = static_cast<ChartContext *>(chart->user_data);
    ChartSeriesContext *series = &ctx->_series_contexts[0];
    lv_coord_t *data = series->_data;
    static bool first = true;

    static auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    int elapsed_s = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    if(elapsed_s > 1) {
      first = true;
      start = std::chrono::steady_clock::now();
    }

    for(size_t i = 0 ; i < series->_num_points; ++i){
        if(first) {
            data[i] = 0;
        }
        else {
    	    data[i] += rand() % 2;
        }
    }
    if(first) {
      first = false;
    }
    lv_chart_set_ext_y_array(chart, series->_lv_series, data);
}

ChartContext::ChartContext(
    const std::string& id, 
    const std::vector<std::string> &xlabels, 
    const std::vector<std::string> &ylabels, 
    const std::string &update_timer_function,
    const std::vector<ChartSeriesContext> &series_contexts,
    lv_obj_t *chart,
    int num_series, 
    int update_hz
  )
  : _type(id)
  , _xlabels(xlabels)
  , _ylabels(ylabels)
  , _update_timer_function(update_timer_function)
  , _series_contexts(series_contexts)
  , _num_series(num_series)
  , _update_timer(
      update_timer_function.size() ?
        lv_timer_create(ChartContext::chartUpdateTimer, 1000 / update_hz, chart) : nullptr)
{}

void 
ChartContext::chartUpdateTimer(struct _lv_timer_t *timer) {
    lv_obj_t *chart = (lv_obj_t *)timer->user_data;
    struct ChartContext *pthis = (struct ChartContext *)chart->user_data;
    //  LOG(DEBUG, BTN, "called in timer (%p) with chart (%p), with chart data (%p)\n", timer, timer->user_data, chart->user_data);
    if (pthis->_update_timer_function.size()) {

        if (pthis->_update_timer_function == "updateChart1") { 
        // osm todo use an array to track these are valid, or register in beginning
        updateChart1(chart);
        }
        // else if (strcmp(ctx->_update_timer_function, "update_functionkey_keystroke_bars") == 0) {
        //   update_functionkey_keystroke_bars(chart);
        // }
    }
}

lv_obj_t *
createChart(
    lv_obj_t *parent,
    const char *id,
    int posx,
    int posy,
    int width,
    int height,
    int ymin,
    int ymax,
    std::vector<std::string> &xlabels,
    std::vector<std::string> &ylabels,
    std::vector<std::string> &series_names,
    std::string update_timer_function,
    const std::string &type,//"bar"
    int update_hz)
{
  if (type != "bar" && type != "line") {
    // LOG(FATAL, BTN, "Unknown chart type specified: '%s'\n",
    //     type && *type ? type : "");
    exit(-1);
  }

  lv_obj_t *chart = lv_chart_create(parent);
  lv_chart_set_type(chart, type == "bar" ?
    LV_CHART_TYPE_BAR : type == "line" ?
    LV_CHART_TYPE_LINE : type == "scatter" ?
    LV_CHART_TYPE_SCATTER : LV_CHART_TYPE_NONE);

  int num_xlabels = xlabels.size();
  int num_ylabels = ylabels.size();
  lv_chart_set_point_count(chart, num_xlabels);

  lv_obj_set_size(chart, width, height);
  lv_obj_align(chart, LV_ALIGN_TOP_LEFT, posx, posy);

  // lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, num_ylabels, 5, num_ylabels, 5, true, 40);
  // lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, num_xlabels, 5, num_xlabels, 1, true, 0);
  lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, ymin, ymax);

  // lv_obj_add_event_cb(chart, onDrawChart, LV_EVENT_DRAW_PART_BEGIN, chart);
  lv_obj_refresh_ext_draw_size(chart);

  // cursor = lv_chart_add_cursor(_chart, lv_palette_main(LV_PALETTE_BLUE), LV_DIR_LEFT | LV_DIR_BOTTOM);

  int num_series = series_names.size();
  std::vector<ChartSeriesContext>series_contexts;
  int cnt;
  for (cnt = 0; cnt < num_series; ++cnt) {
    
    series_contexts.push_back(ChartSeriesContext(
      chart,
      num_xlabels,
      series_names[cnt]));
    lv_chart_set_ext_y_array(
      chart, 
      series_contexts[cnt]._lv_series, 
      series_contexts[cnt]._data);
  }
  struct ChartContext *ctx = new ChartContext(
    id,
    xlabels, 
    ylabels, 
    update_timer_function,
    series_contexts,
    chart,
    num_series, 
    update_hz);
  chart->user_data = ctx;

  lv_obj_t *label = lv_label_create(parent);
  lv_label_set_text(label, "Keyboard Hardware Type: Standard");
  lv_obj_align_to(label, chart, LV_ALIGN_OUT_TOP_MID, 0, -5);

  return chart;
}
