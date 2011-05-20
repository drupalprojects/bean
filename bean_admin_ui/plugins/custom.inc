<?php

/**
 * @file
 * Bean ctools plugin
 */

/**
 * DO NOT USE THIS BEAN.  ONLY USED FOR THE UI PLUGINS
 */
class bean_custom extends bean_plugin {

  /**
   * Delete the record from the database.
   */
  public function delete() {
    db_delete('bean')
      ->condition('type', $this->type)
      ->execute();

    // Delete all fo the beans
    db_delete('bean_type')
      ->condition('type', $this->type)
      ->execute();

    field_attach_delete_bundle('bean', $this->type);

    drupal_flush_all_caches();
  }

  /**
   * Save the record to the database
   */
  public function save($new = FALSE) {
    $bean_type = array(
      'type' => $this->type,
      'label' => $this->getLabel(),
    );

    $primary_key = $new == FALSE ? 'type' : array();
    drupal_write_record('bean_type', $bean_type, $primary_key);
    drupal_flush_all_caches();
  }

  /**
   * Set the label
   *
   * @param label
   */
  public function setLabel($label) {
    $this->plugin_info['label'] = $label;
  }

  public function values() {
    return array();
  }

  public function form($bean) {
    return array();
  }

  public function view($bean, $content, $view_mode = 'full', $langcode = NULL) {
    return $content;
  }
}