<?php

/**
 * @file
 * Views field handler for procurment plan status
 */

/**
 * @class procumrent_plan_handler_field_plan_status
 */
class procurement_plan_handler_field_plan_status extends views_handler_field {

  function init(&$view, &$options) {
    parent::init($view, $options);
    // Don't add the additional fields to groupby
    if (!empty($this->options['allow_toggle'])) {
      $this->additional_fields['ppid'] = array('table' => 'procurement_plan', 'field' => 'ppid');
    }
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['allow_toggle'] = array('default' => TRUE, 'bool' => TRUE);

    return $options;
  }

  /**
   * Provide link to the page being visited.
   */
  function options_form(&$form, &$form_state) {
    $form['allow_toggle'] = array(
      '#title' => t('Allow toggle'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['allow_toggle']),
    );
    parent::options_form($form, $form_state);
  }

  function render($values) {
    $value = t($this->get_value($values));
    $ppid = $this->get_value($values, 'ppid');

    $plan = procurement_plan_load($ppid);

    if (empty($plan)) {
      return ;
    }

    if (!empty($this->options['allow_toggle'])) {
      if (procurement_plan_access('status_toggle', $plan)) {
        $this->options['alter']['make_link'] = TRUE;
        $this->options['alter']['path'] = 'procurement-plan/' . $ppid . '/status-toggle';
        return $this->sanitize_value($value, 'url');
      }
      else {
        return $this->sanitize_value($value);
      }
    }
    else {
      return $this->sanitize_value($value);
    }
  }
}
