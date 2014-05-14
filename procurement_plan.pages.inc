<?php

/**
 * Plan view callback.
 */
function procurement_plan_view($plan) {
  drupal_set_title(entity_label('procurement_plan', $plan));
  return entity_view('procurement_plan', array(entity_id('procurement_plan', $plan) => $plan), 'full');
}
